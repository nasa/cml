/*******************************************************************************

PURPOSE:
   (Provide the capability to initialize the state from a number of potential
   sources.  This class inherits from the JEOD BodyAction class and combines
   the capabilities of JEOD's DynBodyInitState with CML's ORBITAL_ELEMENTS,
   ORB_INIT, and ICOPT models, and CEV's Corr_State_Init_T and
   State_Init_Exec_T structures and the state_init_exec method.)

REFERENCE:
  (This is a reworking of the following models from old CML:
    cml/models/dynamics/state_init_exec/src/state_init_exec.cpp
    cml/models/dynamics/icopt
    cml/dynamics/pom/src/rvmat.c
    cml/models/dynamics/orbital
    cev/models/dynamics/orbital_body
    )

PROGRAMMERS:
   (((Gary Turner) (OSR) (September 2014) (New))
 ******************************************************************************/


#include <cstdlib> // NULL
#include <random> // default_random_engine,
                  // uniform_real_distribution,
                  // normal_distribution


#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"
#include "jeod/models/utils/memory/include/jeod_alloc.hh"
#include "jeod/models/utils/ref_frames/include/ref_frame.hh"
#include "jeod/models/dynamics/body_action/include/body_action_messages.hh"
#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"

#include "../include/state_initialize.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
StateInitialize::StateInitialize()
  :
  // public values:
  position_input_data_type(Unspecified),
  velocity_input_data_type(Unspecified),
  attitude_input_data_type(Unspecified),
  att_rate_input_data_type(Unspecified),
  orb_elem_init(),
  trans_init(),
  rot_init(),
  ned_rot_init(),
  correlation(),
  monte_carlo_dispersion(trans_init),
  TR_param(),
  planet_name(),
  ref_point_altitude(0.0),
  ref_point_latitude(0.0),
  ref_point_longitude(0.0),
  TR_geodetic_altitude(0.0),
  TR_theta_Rng(0.0),
  TR_theta_Rot(0.0),
  TR_phi_Cross(0.0),
  rot_ref_latitude(0.0),
  rot_ref_longitude(0.0),
  rot_ref_azimuth(0.0),
  speed(0.0),
  flight_path_angle(0.0),
  azimuth(0.0),
  TR_Lambda(0.0),
  yaw(0.0),
  roll(0.0),
  pitch(0.0),
  angle_of_bank(0.0),
  angle_of_sideslip(0.0),
  angle_of_attack(0.0),
  T_inrtl_body{{1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{0.0, 0.0, 1.0}},
  ref_body_sequence_override(jeod::Orientation::NoSequence),
  roll_rate_body(0.0),
  pitch_rate_body(0.0),
  yaw_rate_body(0.0),
  max_rate_random(0.0),
  att_random_seed(1),
  att_rate_random_seed(2),
  free_stream_velocity{0.0, 0.0, 0.0},
  // protected values:
  initialized(false),
  planet(NULL),
  use_orbital_init(false),
  use_trans_init(true),
  use_rot_init(true),
  use_ned_rot_init(false),
  use_trans_init_passthrough(false),
  use_rot_init_passthrough(false),
  use_pfix_frame_trans(false),
  pfix_ref_point_state_generated(false),
  use_veh_position_as_reference(false),
  populate_trans_init_pos_from_pfix(false),
  populate_trans_init_vel_from_pfix(false),
  velocity_is_relative_to_inertial(false),
  requires_free_stream_velocity(false),
  populate_rot_init_transform_source(RITS_None),
  z_axis_points_up(false),
  increment_rot_rate_init_with_pfix(false),
  pfix_position{0.0, 0.0, 0.0},
  pfix_velocity{0.0, 0.0, 0.0},
  omega_cross_r{0.0, 0.0, 0.0},
  T_pfix_reference{{1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{0.0, 0.0, 1.0}},
  T_inrtl_reference{{1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{0.0, 0.0, 1.0}},
  T_reference_body{{1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{0.0, 0.0, 1.0}},
  E_pfix_reference{0.0, 0.0, 0.0},
  E_reference_body{0.0, 0.0, 0.0},
  random_value(0.0),
  random_unit_vector{0.0, 0.0, 0.0},
  force_match_trans(false),
  force_match_rot(false)
{}


/*****************************************************************************
initialize
Purpose:(Initializes the BodyAction and contained body actions.)
Reference: (cml/models/dynamics/state_init_exec/src/state_init_exec.cpp:80:215
            Note - the options:
            INIT_EXEC_PRE_ORB_INIT is not implemented
            INIT_EXEC_PRE_ICOPT is covered by options except OrbitalElements
            INIT_EXEC_PRE_OE2CAR is covered by option OrbitalElements.
            )
*****************************************************************************/
void
StateInitialize::initialize(
      jeod::DynManager & dyn_manager)
{

  // find_planet is a DynBodyInit method, accessible by inheritance.
  planet = find_planet (dyn_manager, planet_name, "planet_name");
  // Method fails if planet pointer is NULL.  but just to be sure (in case the
  // DynBodyInit method changes) check it again here.  Note, however, it is
  // not possible to test this line of code.
  if (planet == NULL) {
    CMLMessage::fail(
      __FILE__, __LINE__, "Invalid configuration.\n",
      "Action named '", action_identifier, "' failed:\n" "Could not find planet named '", planet_name, "'");
    return;
  }

  // The subject for any state initialization action must be a DynBody, not a
  // MassBody. This pointer will also be null if a subject was never set.
  auto* subject = get_subject_dyn_body();
  if (subject == nullptr) {
    if (mass_subject != nullptr) {
      CMLMessage::fail(
        __FILE__, __LINE__, "Invalid configuration.\n",
        "The subject provided must be a DynBody, not a MassBody.");
    } else {
      CMLMessage::fail(
        __FILE__, __LINE__, "Invalid configuration.\n",
        "A subject was never provided.");
    }
    return;
  }

  // Here we are copying the reference frame names to strings within this
  //       StateInitialize object, as opposed to passing their pointers directly
  //       into trans_init and rot_init. This is to avoid a possible scenario in
  //       which planet is changed or the object it points to is deleted, which
  //       would deallocate the location of the names and potentially break
  //       trans_init and rot_init if the names were not copied.
  reference_ref_frame_name_inertial = planet->inertial.get_name();
  reference_ref_frame_name_pfix = planet->pfix.get_name();

  // reference_ref_frame_name is inherited from parent class (DynBodyInit).
  // It must be set to prevent an error in a function in the parent class
  // (DynBodyInit::initialize) even though it isn't actually used directly --
  // the specific instances (trans_init, rot_init, etc.) are used instead, but
  // these may be populated from this value.
  reference_ref_frame_name = reference_ref_frame_name_inertial;

  // set planet pointer for target relative state init options:
  //       TR_GeodAlt_RngAng_RotAng, TR_GeodAlt_RngAng_CrossAng*,
  //       TR_InertialSpeedGammaLatang
  TR_param.set_planet(planet);

  // set planet pointer for correlated dispersion option: PV_COVAR_TR_PARAM
  correlation.TR_param.set_planet(planet);

  DynBodyInit::initialize( dyn_manager );

  select_position_initializer();
  select_velocity_initializer();
  select_attitude_initializer();
  select_att_rate_initializer();
  verify_compatibility();

  if (use_orbital_init) {
    orb_elem_init.action_name = action_name;
    orb_elem_init.set_subject_body(*subject);
    orb_elem_init.body_frame_id = body_frame_id;
    orb_elem_init.reference_ref_frame_name = reference_ref_frame_name;
    orb_elem_init.planet_name = planet_name;
    orb_elem_init.orbit_frame_name = reference_ref_frame_name;
    orb_elem_init.initialize( dyn_manager);
    // If the state is to have correlations/dispersions added, then use the
    // orb-elem-generated state to populate trans_init, so configure
    // trans_init for post-dispersion application.
    // NOTE
    //  - Do not set use_trans_init=true at this point; we will want to
    //    populate trans_init (and THEN set use_trans_init=true) AFTER
    //    orb_elem_init has populated the state so that the orb-elem-defined
    //    nominal state can then be dispersed and then reapplied by trans-init.
    //  - set the ref-frame name for trans-init regardless because it saves an
    //    if-check later in the model
    trans_init.reference_ref_frame_name = reference_ref_frame_name;
    if ( correlation.dispersion_distribution !=
                             CorrelatedStateDispersion::NoDispersion ||
        monte_carlo_dispersion.disperse_pv){
      trans_init.action_name = action_name;
      trans_init.body_frame_id = body_frame_id;
      trans_init.set_subject_body(*subject);
      trans_init.initialize( dyn_manager);
    }
  }
  if (use_trans_init) {
    if (!use_trans_init_passthrough) {
      trans_init.action_name = action_name;
      trans_init.reference_ref_frame_name = use_pfix_frame_trans ?
                    reference_ref_frame_name_pfix : reference_ref_frame_name;
      trans_init.body_frame_id = body_frame_id;
    }
    trans_init.set_subject_body(*subject);
    trans_init.initialize( dyn_manager);
  }
  if (use_rot_init) {
    if (!use_rot_init_passthrough) {
      rot_init.action_name = action_name;
      rot_init.reference_ref_frame_name = reference_ref_frame_name;
      rot_init.body_frame_id = body_frame_id;
    }
    rot_init.set_subject_body(*subject);
    rot_init.initialize( dyn_manager);
  }
  if (use_ned_rot_init) {
    ned_rot_init.set_subject_body(*subject);
    ned_rot_init.action_name = action_name;
    ned_rot_init.reference_ref_frame_name = reference_ref_frame_name;
    ned_rot_init.body_frame_id = body_frame_id;
    ned_rot_init.planet_name = planet_name;
    ned_rot_init.initialize( dyn_manager);
  }
  initialized = true;
}

/*****************************************************************************
select_position_initializer
Purpose:(directs the flow through the initialization options)
*****************************************************************************/
void
StateInitialize::select_position_initializer()
{
  switch(position_input_data_type) {
   case TransInit:
    use_trans_init_passthrough = true;
    force_match_trans = (velocity_input_data_type != Off);
    break;

   case Inertial:
   case ICOPT_R_INERTIAL:
    // Copy the local position into the TransInit class.
    jeod::Vector3::copy(position, trans_init.position);
    break;

   case Pfix:
    use_pfix_frame_trans = true;
    force_match_trans = (velocity_input_data_type != Off);
    jeod::Vector3::copy(position, trans_init.position);
    break;

   case NED_Geodetic:
   case GEOD_LAT_LON_ALT:
    pfix_ref_point_state.altlatlong_type = jeod::NorthEastDown::elliptical;
    generate_pfix_position();
    break;

   case NED_Geocentric:
    pfix_ref_point_state.altlatlong_type = jeod::NorthEastDown::spherical;
    generate_pfix_position();
    break;

   case OrbitalElements:
    use_orbital_init = true;
    use_trans_init = false;
    force_match_trans = (velocity_input_data_type != Off);
    remove_from_trans_init( jeod::DynBodyInitTransState::Position);
    break;

   case TR_GeodAlt_RngAng_RotAng:
    populate_trans_init_pos_from_pfix = true;
    TR_param.initialize( ref_point_altitude,
                         ref_point_latitude,
                         ref_point_longitude);
    // generate pfix_position 3-array from TR-param configuration and
    // specified altitude, range-angle, rotation-angle.
    TR_param.compute_position_from_params( TR_geodetic_altitude,
                                           TR_theta_Rng,
                                           TR_theta_Rot,
            TargetRelative_StateParameter::INPUT_THETA_ROT,
                                           pfix_position);
    break;

   case TR_GeodAlt_RngAng_CrossAng_AbsRotAng_LT_90:
    populate_trans_init_pos_from_pfix = true;
    TR_param.initialize( ref_point_altitude,
                         ref_point_latitude,
                         ref_point_longitude);
    TR_param.compute_position_from_params( TR_geodetic_altitude,
                                           TR_theta_Rng,
                                           TR_phi_Cross,
            TargetRelative_StateParameter::INPUT_PHI_ABS_THETA_ROT_LT_90,
                                           pfix_position);
    break;

   case TR_GeodAlt_RngAng_CrossAng_AbsRotAng_GT_90:
    populate_trans_init_pos_from_pfix = true;
    TR_param.initialize( ref_point_altitude,
                         ref_point_latitude,
                         ref_point_longitude);
    TR_param.compute_position_from_params( TR_geodetic_altitude,
                                           TR_theta_Rng,
                                           TR_phi_Cross,
            TargetRelative_StateParameter::INPUT_PHI_ABS_THETA_ROT_GT_90,
                                           pfix_position);
    break;

   case Off:
    remove_from_trans_init( jeod::DynBodyInitTransState::Position);
    break;

   case Unspecified:
    CMLMessage::fail (
      __FILE__, __LINE__, jeod::BodyActionMessages::illegal_value, "\n",
       action_identifier, " failed:\n"
      "Position init option not specified.  Use 'Off' to turn off");
    break;

   default:
    CMLMessage::fail (
      __FILE__, __LINE__, jeod::BodyActionMessages::illegal_value, "\n",
       action_identifier, " failed:\n"
      "Illegal position init option specified");
  }
}



/*****************************************************************************
select_velocity_initializer
Purpose:(directs the flow through the initialization options)
*****************************************************************************/
void
StateInitialize::select_velocity_initializer()
{
  double vel_XY = 0.0;

  switch (velocity_input_data_type) {
   case TransInit:
    use_trans_init_passthrough = true;
    force_match_trans = force_match_trans || (position_input_data_type != Off);
    break;

   case Inertial:
   case ICOPT_V_INERTIAL:
    // Copy the local velocity into the TransInit class.
    jeod::Vector3::copy(velocity, trans_init.velocity);
    // If TransInit is not being used for velocity, change the state_items
    // from Both (default) to position-only.
    break;

   case Pfix:
    use_pfix_frame_trans = true;
    force_match_trans = force_match_trans || (position_input_data_type != Off);
    jeod::Vector3::copy(velocity, trans_init.velocity);
    break;

   case NED_Geodetic:
    pfix_ref_point_state.altlatlong_type = jeod::NorthEastDown::elliptical;
    generate_pfix_velocity();
    break;

   case NED_Geocentric:
    pfix_ref_point_state.altlatlong_type = jeod::NorthEastDown::spherical;
    generate_pfix_velocity();
    break;

   case  InertialSpeedGammaAzimuth :
   case  ICOPT_INERT_VEL_GAM_AZ :
    velocity_is_relative_to_inertial = true;
    // fall through, or go direct with
    // default velocity_is_relative_to_inertial = false
    [[fallthrough]];
   case  PfixSpeedGammaAzimuth :
   case  ICOPT_REL_VEL_GAM_AZ :
    vel_XY = speed * std::cos(flight_path_angle);
    velocity[0] = vel_XY * std::cos(azimuth);
    velocity[1] = vel_XY * std::sin(azimuth);
    velocity[2] = -speed * std::sin(flight_path_angle);
    use_veh_position_as_reference = true;
    pfix_ref_point_state.altlatlong_type = jeod::NorthEastDown::spherical;
    break;

   case OrbitalElements:
    use_orbital_init = true;
    use_trans_init = false;
    force_match_trans = force_match_trans || (position_input_data_type != Off);
    remove_from_trans_init( jeod::DynBodyInitTransState::Velocity);
    break;

   case TR_InertialSpeedGammaLatang:
    // Note: Set "use_veh_position_as_reference" to force the call to
    //       generate_pfix_velocity_from_veh_pos() when the trans-state initial
    //       conditions are getting computed.  This will generate a velocity
    //       expressed in the pfix frame (via the TR-state-params methods).
    //       Also set "velocity_is_relative_to_inertial" to indicate that the
    //       velocity so generated is wrt inertial (while expressed in pfix).
    use_veh_position_as_reference     = true;
    velocity_is_relative_to_inertial  = true;
    TR_param.initialize( ref_point_altitude,
                         ref_point_latitude,
                         ref_point_longitude);
    break;

   case Off:
    remove_from_trans_init( jeod::DynBodyInitTransState::Velocity);
    break;

   case Unspecified:
    CMLMessage::fail (
      __FILE__, __LINE__, jeod::BodyActionMessages::illegal_value, "\n",
       action_identifier, " failed:\n"
      "Velocity init option not specified.  Use 'Off' to turn off");
    break;

   default:
    CMLMessage::fail (
      __FILE__, __LINE__, jeod::BodyActionMessages::illegal_value, "\n",
       action_identifier, " failed:\n"
      "Illegal velocity init option specified");
  }
}


/*****************************************************************************
select_attitude_initializer
Purpose:(directs the flow through the initialization options)
*****************************************************************************/
void
StateInitialize::select_attitude_initializer()
{
  // Pass through the switch twice; there is a great deal of duplicated code
  // otherwise.
  // First pass for E_reference_body
  switch (attitude_input_data_type) {
   case EulerInertialVelvec_YPR:
   case SunPointing_YPR:
   case PfixUpCompAzimuth:
   case PfixUpCompAzimuth_YPR:
   case EulerBoostRef_YPR:
   case ICOPT_EUL_YPR_BR: // Boost reference
   case EulerInertial_YPR:
   case ICOPT_EUL_YPR_INERT_BODY:
   case EulerNED_YPR:
    // For these cases, need to set the inertial-reference orientation after
    // the trans_init has been applied, so these are not set here.
    // Set the Euler angles from reference to body frame.
    E_reference_body[0] = yaw;
    E_reference_body[1] = pitch;
    E_reference_body[2] = roll;
    ref_body_sequence = jeod::Orientation::Yaw_Pitch_Roll;
    break;
   case SunPointingEcliptic_PYR:
   case EulerPlumbline_PYR:
   case ICOPT_EUL_PYR_PL: // plumbline
    E_reference_body[0] = pitch;
    E_reference_body[1] = yaw;
    E_reference_body[2] = roll;
    ref_body_sequence = jeod::Orientation::Pitch_Yaw_Roll;
    break;
   case EulerFreestream_RYP:
   case ICOPT_EUL_RYP_FS:
    // Set the Euler angles from reference to body frame.
    E_reference_body[0] = angle_of_bank;
    E_reference_body[1] = -angle_of_sideslip;
    E_reference_body[2] = angle_of_attack;
    ref_body_sequence = jeod::Orientation::Roll_Yaw_Pitch;
    break;
   default:
    break; // No action, this covers options that do not need E_reference_body.
           // Note - "Unspecified" or illegal values will fail at the next
           //        switch.
  }
  // Check for an override of the default sequence.
  if (ref_body_sequence_override != jeod::Orientation::NoSequence) {
    CMLMessage::inform(
      __FILE__,__LINE__,"Modified Euler Sequence\n",
      "Euler sequence for orientation type ", attitude_input_data_type, " is being changed from the "
      "default ", ref_body_sequence, " to the manual override ", ref_body_sequence_override, ".\n");
    ref_body_sequence = ref_body_sequence_override;
  }

  // Now specific cases:
  switch (attitude_input_data_type) {
   case RotInit:
    use_rot_init_passthrough = true;
    // force a match with the same attitude rate initializer if
    // the attitude rate initializer is not Off.  I.e. cannot use this option
    // if the attitude rate initializer uses any other legitimate option.
    // This may seem like a radical requirement, but the other methods
    // configure the rot_init object for their own purposes, overriding the
    // settings that may be given to rot_init by the user.
    // Modification:  RotInit_NED also turns rot_init off, so is an acceptable
    //                bypass alternative.
    force_match_rot = (att_rate_input_data_type != Off &&
                       att_rate_input_data_type != RotInit_NED);
    break;

   case EulerInertialVelvec_YPR:
    populate_rot_init_transform_source = RITS_VelPos;
    z_axis_points_up = true;
    break;

   case SunPointing_YPR:
   case SunPointingEcliptic_PYR:
    // Need to use the trans-init position to generate the sun-earth
    // position vector.  If that is not specified in the correct frame
    // (possible if using use_trans_init_passthrough or pfix initialization),
    // the algebra will not work out.
    // NOTE trans_init.reference_ref_frame_name requires manual setting if
    //      using a passthrough option and may not have been set at all if
    //      using an orbital-elements specification.
    if( use_trans_init_passthrough &&
           (!reference_ref_frame_name_inertial.compare(
                                 trans_init.reference_ref_frame_name))) {
      CMLMessage::fail (
        __FILE__, __LINE__, "StateInitialization::invalid_configuration\n",
        "Rot init is being configured using the sun-vehicle vector.\n"
        "That requires the vehicle position wrt the initializing planet.\n"
        "This must be expressed in planet-relative inertial.\n"
        "Expecting ", reference_ref_frame_name_inertial, " but the specified frame name is ", trans_init.reference_ref_frame_name, ".\n");
    }
    if( use_pfix_frame_trans) { // which defaults to false
      CMLMessage::fail (
        __FILE__, __LINE__, "StateInitialization::invalid_configuration\n",
        "Rot init is being configured using the sun-vehicle vector.\n"
        "That requires the vehicle position wrt the initializing planet.\n"
        "This must be expressed in planet-relative inertial but the\n"
        "trans-init is specified in pfix.\n");
    }
    // Now split on which option:
    if (attitude_input_data_type == SunPointing_YPR) {
      populate_rot_init_transform_source = RITS_SolarPosPos;
      z_axis_points_up = true;
    } else {
      populate_rot_init_transform_source = RITS_SolarPosEclipticPole;
    }
    break;

   case PfixUpCompAzimuth:
   case PfixUpCompAzimuth_YPR:
    // Set the Euler angles from planet-fix to reference.
    // Reference in this case is (Up - East - North) and uses the same
    // reference point as the position and velocity specifications.
    E_pfix_reference[0] = ref_point_longitude;
    E_pfix_reference[1] = -ref_point_latitude;
    E_pfix_reference[2] = azimuth;
    jeod::Orientation::compute_matrix_from_euler_angles( jeod::Orientation::Yaw_Pitch_Roll,
                                                   E_pfix_reference,
                                                   T_pfix_reference);
    // note - this technique for attitude can be used with other
    //        attitude-rates, but this technique for attitude rates requires
    //        this technique for attitude.
    populate_rot_init_transform_source = RITS_PfixRef;
    break;

   case EulerNED_YPR:
    // Set the Euler angles from planet-fix to reference.
    // Reference in this case is (North - East - Down) and uses the same
    // reference point as the position and velocity specifications.
    E_pfix_reference[0] = ref_point_longitude;
    E_pfix_reference[1] = -(M_PI_2 + ref_point_latitude);
    E_pfix_reference[2] = 0.0;
    jeod::Orientation::compute_matrix_from_euler_angles( jeod::Orientation::Yaw_Pitch_Roll,
                                                   E_pfix_reference,
                                                   T_pfix_reference);
    // note - this technique for attitude can be used with other
    //        attitude-rates, but this technique for attitude rates requires
    //        this technique for attitude.
    populate_rot_init_transform_source = RITS_PfixRef;
    break;


   case EulerBoostRef_YPR:
   case ICOPT_EUL_YPR_BR: // Boost reference
    // Set the Euler angles from planet-fix to reference.
    E_pfix_reference[0] = rot_ref_longitude;
    E_pfix_reference[1] = -(M_PI_2 + rot_ref_latitude);
    E_pfix_reference[2] = 0.0;
    jeod::Orientation::compute_matrix_from_euler_angles( jeod::Orientation::Yaw_Pitch_Roll,
                                                   E_pfix_reference,
                                                   T_pfix_reference);
    populate_rot_init_transform_source = RITS_PfixRef;
    break;

   case EulerPlumbline_PYR:
   case ICOPT_EUL_PYR_PL: // plumbline
    // Set the Euler angles from planet-fix to reference.
    E_pfix_reference[0] = rot_ref_longitude;
    E_pfix_reference[1] = -rot_ref_latitude;
    E_pfix_reference[2] = -rot_ref_azimuth;
    jeod::Orientation::compute_matrix_from_euler_angles( jeod::Orientation::Yaw_Pitch_Roll,
                                                   E_pfix_reference,
                                                   T_pfix_reference);
    populate_rot_init_transform_source = RITS_PfixRef;
    break;

   case EulerFreestream_RYP:
   case ICOPT_EUL_RYP_FS:
    // The inertial wind velocity, necessary for generating the free-stream
    // velocity, is not known until after the vehicle position has been
    // initialized (among other steps).
    // This option requires an additional step; the  method
    //       overwrite_attitude_from_free_stream( double rel_wind_inrtl[3])
    // must be called AFTER the wind velocity is available.
    // For now, the attitude will be initialized with the assumption that the
    // wind velocity (relative to ground) is zero.
    requires_free_stream_velocity = true;
    populate_rot_init_transform_source = RITS_VelPos;
    break;

   case MatrixInertialBody:
   case ICOPT_INERT_BODY_ROTMAT:
    jeod::Matrix3x3::copy(T_inrtl_body,
                    rot_init.orientation.trans);
    rot_init.orientation.data_source = jeod::Orientation::InputMatrix;
    break;

   case EulerInertial_YPR:
   case ICOPT_EUL_YPR_INERT_BODY: // Populate E_reference_body as YPR
    populate_rot_init_transform_source = RITS_Inertial;
    break;

   case Random:
    // Generate a uniformly-distributed random attitude. This is done by
    // rotating so that the x-axis is uniformly randomly oriented, then rotating
    // about the x-axis by a uniformly-distributed random angle. See
    // docs/state_initialize.pdf for a more detailed explanation.
   {
    generate_random(att_random_seed);
    random_value *= M_PI; // Half of the random rotation angle about the x-axis
    double sinhalf = std::sin(random_value), coshalf = std::cos(random_value);
    double temp = std::sqrt(2 * (random_unit_vector[0] + 1));
    jeod::Quaternion random_att;
    // random_unit_vector is the new orientation of the x-axis
    if (MathUtils::is_near_equal(temp, 0.0)) { // Avoid dividing by 0
      random_att.scalar    = 0.0;
      random_att.vector[0] = 0.0;
      random_att.vector[1] = sinhalf;
      random_att.vector[2] = coshalf;
    }
    else {
      random_att.scalar    = temp/2 * coshalf;
      random_att.vector[0] = temp/2 * sinhalf;
      random_att.vector[1] =
         (random_unit_vector[1]*sinhalf - random_unit_vector[2]*coshalf) / temp;
      random_att.vector[2] =
         (random_unit_vector[1]*coshalf + random_unit_vector[2]*sinhalf) / temp;
    }
    rot_init.orientation.set_quaternion(random_att);
    rot_init.orientation.data_source = jeod::Orientation::InputQuaternion;
   }
    break;

   case RotInit_NED:
    // turn on the rot-init-ned option.  If it is to be used only for attitude,
    // configure as such (default is to use for both attitude and attitude
    // rate).  Finally, remove attitude from the regular rot-init
    // responsibilities.
    use_ned_rot_init = true;
    if (att_rate_input_data_type != RotInit_NED) {
      ned_rot_init.set_items = jeod::RefFrameItems::Att;
    }
    // fallthrough
   case Off:
    remove_from_rot_init( jeod::DynBodyInitRotState::Attitude);
    break;

   case Unspecified:
    CMLMessage::fail (
      __FILE__, __LINE__, jeod::BodyActionMessages::illegal_value, "\n",
       action_identifier, " failed:\n"
      "Attitude init option not specified.  Use 'Off' to turn off");
    break;

   default:
    CMLMessage::fail (
      __FILE__, __LINE__, jeod::BodyActionMessages::illegal_value, "\n",
       action_identifier, " failed:\n"
      "Illegal attitude init option specified");
  }
}

/*****************************************************************************
select_att_rate_initializer
Purpose:(directs the flow through the initialization options)
*****************************************************************************/
void
StateInitialize::select_att_rate_initializer()
{
  switch (att_rate_input_data_type) {
   case RotInit:
    // force a match with the same attitude initializer if
    // the attitude initializer is anything other than Off of RotInit_NED.
    // i.e. cannot use this option if the attitude initializer uses any
    // option that may reconfigure rot_init for its own purposes.
    use_rot_init_passthrough = true;
    force_match_rot = force_match_rot ||
                      (attitude_input_data_type != Off &&
                       attitude_input_data_type != RotInit_NED);
    break;

   case PfixUpCompAzimuth:
   case PfixUpCompAzimuth_YPR:
   case EulerNED_YPR:
   case BodyPfixRate:
    increment_rot_rate_init_with_pfix = true;
    // fallthrough to populate rot_init.ang_velocity.
    [[fallthrough]];

   case BodyInertialRate:
    // These are the body rates wrt the specified frame, expressed in body.
    rot_init.ang_velocity[0] = roll_rate_body;
    rot_init.ang_velocity[1] = pitch_rate_body;
    rot_init.ang_velocity[2] = yaw_rate_body;
    break;

   case Random:
    generate_random(att_rate_random_seed);
    random_value *= max_rate_random;
    jeod::Vector3::scale( random_unit_vector,
                    random_value,
                    rot_init.ang_velocity);
    break;

   case RotInit_NED:
    // turn on the rot-init-ned option.  If it is to be used only for
    // attitude-rate, configure as such (default is to use for both attitude
    // and attitude-rate).  Finally, remove attitude-rate from the regular
    // rot-init responsibilities.
    use_ned_rot_init = true;
    if (attitude_input_data_type != RotInit_NED) {
      ned_rot_init.set_items = jeod::RefFrameItems::Rate;
    }
    // fallthrough
   case Off:
    remove_from_rot_init( jeod::DynBodyInitRotState::Rate);
    break;

   case Unspecified:
    CMLMessage::fail (
      __FILE__, __LINE__, jeod::BodyActionMessages::illegal_value, "\n",
       action_identifier, " failed:\n"
      "Attitude-rate init option not specified.  Use 'Off' to turn off");
    break;

   default:
    CMLMessage::fail (
      __FILE__, __LINE__, jeod::BodyActionMessages::illegal_value, "\n",
       action_identifier, " failed:\n"
      "Illegal attitude-rate init option specified");
  }
}


/*****************************************************************************
apply
Purpose:(Applies the state_initialize action)
*****************************************************************************/
void
StateInitialize::apply(
   jeod::DynManager & dyn_manager)
{
  if (!initialized) {
    CMLMessage::error(
      __FILE__,__LINE__,"Premature application of Body Action.\n"
      "This body-action ('", action_identifier, "') must be initialized before it can be applied.\n"
      "Application failed.\n");
    return;
  }

  if (use_orbital_init) {
    generate_orbital_init_values( dyn_manager);
    //includes call to DynBodyInitOrbit::apply
  }
  if (use_trans_init) { // Not an elseif; orbital_init may have turned this on
    generate_trans_init_values();
  }
  // apply MonteCarlo dispersions to the nominal state.  Note - the
  // translational state may be a necessary component in setting the
  // state of the frames used to define the rotational state.  So apply these
  // dispersions beforegenerating the rotational state.
  if (monte_carlo_dispersion.disperse_pv) {
    if ( correlation.dispersion_distribution !=
                              CorrelatedStateDispersion::NoDispersion) {
      CMLMessage::warn(
        __FILE__,__LINE__,"Simultaneous application of dispersions\n",
        "The configuration calls for application of 2 independent dispersions"
        "\napplied to the nominal state.\n"
        "The first to be applied will be a MonteCarlo direct-dispersion of\n"
        "state on independent axes. This could be random or file-driven but\n"
        "effectively treats each of the state components independently.\n"
        "The second to be applied uses a covariance matrix.  This dispersion\n"
        "will be applied onto the state resulting from the first dispersion.\n"
        "Check configuration to ensure that this is the intended setup.\n");
    }
    // Note -- apply direct dispersion first to support analysis along a short
    //         arc around the nominal state.  This can move the vehicle to a
    //         point on the arc (defined in a file), then apply a
    //         correlated-dispersion to that state.
    monte_carlo_dispersion.disperse_trans_state();
  }

   if (use_rot_init) {
    generate_rot_init_values( dyn_manager);
  }

  // apply correlations
  if ( correlation.dispersion_distribution !=
                              CorrelatedStateDispersion::NoDispersion) {
    if ( correlation.corr_base_frame_name.empty() ||
         correlation.corr_base_frame_name == trans_init.reference_ref_frame_name) {
      correlation.disperse_state( trans_init.position,
                                  trans_init.velocity,
                                  rot_init.orientation.trans);
    }
    else {
      // find ref_frame is a DynBodyInit method, accessible by inheritance
      jeod::RefFrame * corr_base = find_ref_frame( dyn_manager,
                                             correlation.corr_base_frame_name,
                                             "ref-frame name");
                         
      // find ref_frame is a DynBodyInit method, accessible by inheritance
      jeod::RefFrame * trans_ref = find_ref_frame( dyn_manager,
                                             trans_init.reference_ref_frame_name,
                                             "ref-frame name");

      // Compute relative rotation from corr_base to trans_base
      jeod::RefFrame * rot_ref = find_ref_frame( dyn_manager,
                                           rot_init.reference_ref_frame_name,
                                           "ref-frame name");

      // NULL checked in find_ref_frame
      dyn_manager.subscribe_to_frame(*corr_base);
      dyn_manager.subscribe_to_frame(*trans_ref);
      dyn_manager.subscribe_to_frame(*rot_ref);
      dyn_manager.update_ephemerides();
      dyn_manager.unsubscribe_to_frame(*corr_base);
      dyn_manager.unsubscribe_to_frame(*trans_ref);
      dyn_manager.unsubscribe_to_frame(*rot_ref);

      // Compute relative translation from corr_base to trans_base
      jeod::RefFrameState trans_init_wrt_corr_base;
      trans_ref->compute_relative_state( *corr_base,
                                          trans_init_wrt_corr_base);

      if (trans_init.reference_ref_frame_name == rot_init.reference_ref_frame_name) {
        correlation.disperse_state( trans_init_wrt_corr_base,
                                    trans_init.position,
                                    trans_init.velocity,
                                    rot_init.orientation.trans);
      }
      else {
        jeod::RefFrameState rot_init_wrt_corr_base;
        rot_ref->compute_relative_state( *corr_base,
                                          rot_init_wrt_corr_base);
                         
        correlation.disperse_state( trans_init_wrt_corr_base,
                                    rot_init_wrt_corr_base.rot.T_parent_this,
                                    trans_init.position,
                                    trans_init.velocity,
                                    rot_init.orientation.trans);
      }
    }
  }


  // Make the actual applications at this point.
  // Separating off to make this injection point available to derivations
  // of this class.
  apply_internal( dyn_manager);
}


/*****************************************************************************
apply_internal
Purpose:(applies the appropriate body-actions)
*****************************************************************************/
void
StateInitialize::apply_internal(
   jeod::DynManager & dyn_manager)
{
  // apply body actions:
  if (use_trans_init) {
    trans_init.apply( dyn_manager);
  }
  if (use_rot_init) {
    rot_init.apply( dyn_manager);
  }
  if (use_ned_rot_init) {
    ned_rot_init.apply( dyn_manager);
  }

  DynBodyInit::apply( dyn_manager );
}




/*****************************************************************************
generate_orbital_init_values
Purpose:(Generates the data for DynBodyInitOrbit)
*****************************************************************************/
void
StateInitialize::generate_orbital_init_values(
       jeod::DynManager & dyn_manager)
{
  // The validity of the subject dyn_body pointer was checked in initialize().
  auto* subject = get_subject_dyn_body();

  // If one of the states is NOT to be initialized this way, store off the
  // current value before it gets overwritten.
  double temp_state[3];
  if (position_input_data_type == Off) {
    jeod::Vector3::copy(subject->composite_body.state.trans.position,
                  temp_state);
  }
  else if (velocity_input_data_type == Off) {
    jeod::Vector3::copy(subject->composite_body.state.trans.velocity,
                  temp_state);
  }

  orb_elem_init.apply( dyn_manager);

  // Then reinstate the state values that were just unintentionally copied
  // over.
  if (position_input_data_type == Off) {
    jeod::Vector3::copy( temp_state,
                   subject->composite_body.state.trans.position);
  }
  else if (velocity_input_data_type == Off) {
    jeod::Vector3::copy( temp_state,
                   subject->composite_body.state.trans.velocity);
  }

  // Populate the trans_init values in case they are needed by rot_init or
  // by the dispersion generator.
  // Notes:
  // -- In some situations, it may be necessary to have accurate
  //    trans_init.position and trans_init.velocity values to generate
  //    frame transformations, e.g. Lvlh-to-inertial.
  // -- copy position value if trans-init is not being used or if it is only
  //    being used for initializing velocity values.  In this case,  DO NOT
  //    copy the velocity values coming from orbital parameter initialization
  //    because that would overwrite the desired velocity values.
  // -- Similarly, copy velocity values if trans-init is not being used or if
  //    it is only being used for initializing position values.
  // -- Copying in a position value when trans-init is only being used for
  //    velocity (and vice-versa) has no effect on the state initialization.
  //    Trans-init is smart enough to ignore position values when
  //    state_items = jeod::DynBodyInitTransState::velocity (and vice-versa).
  if (!use_trans_init ||
      trans_init.state_items == jeod::DynBodyInitTransState::Velocity) {
    jeod::Vector3::copy(subject->composite_body.state.trans.position,
                  trans_init.position);
  }
  if (!use_trans_init ||
      trans_init.state_items == jeod::DynBodyInitTransState::Position) {
    jeod::Vector3::copy(subject->composite_body.state.trans.velocity,
                  trans_init.velocity);
  }
  // If the state is to have correlated or direct dispersions added, then
  // set trans_init back in effect.
  if ( correlation.dispersion_distribution !=
                           CorrelatedStateDispersion::NoDispersion ||
      monte_carlo_dispersion.disperse_pv){
    trans_init.state_items = jeod::DynBodyInitTransState::Both;
    use_trans_init = true;
  }
}


/*****************************************************************************
generate_trans_init_values
Purpose:(Generates the data for jeod::DynBodyInitTransState)
*****************************************************************************/
void
StateInitialize::generate_trans_init_values()
{
  // Position, one optional computation
  if (populate_trans_init_pos_from_pfix) {
    jeod::Vector3::transform_transpose(planet->pfix.state.rot.T_parent_this,
                                 pfix_position,
                                 trans_init.position);
  }
  // Could be using the trans_init position value to initialize other parts
  // of the state. If position is initialized elsewhere, need that value
  // copying into trans_init.position so that the other elements can still
  // access it.
  else if ( trans_init.state_items == jeod::DynBodyInitTransState::Velocity) {
    jeod::Vector3::copy( get_subject_dyn_body()->composite_body.state.trans.position,
                   trans_init.position);
  }
  // Velocity, two options, potentially sequential:
  if (use_veh_position_as_reference) {
    generate_pfix_velocity_from_veh_pos();
  }
  if (populate_trans_init_vel_from_pfix) {
    generate_inertial_velocity();
  }
}


/*****************************************************************************
generate_rot_init_values
Purpose:(Generates the data for jeod::DynBodyInitRotState)
*****************************************************************************/
void
StateInitialize::generate_rot_init_values(
       jeod::DynManager & dyn_manager)
{
  if (requires_free_stream_velocity) {
    double scratch_vec[3];
    jeod::Vector3::initialize(scratch_vec);
    // Don't know the wind velocity, but need something.  Generate the
    // free-stream velocity (temporarily) assuming a zero wind velocity.
    // This can be corrected with a call to
    // overwrite_attitude_from_free_stream( wind_vel[3])
    // once the wind velocity is known.
    generate_free_stream_velocity(scratch_vec);
  }

  // Now generate the orientation of the reference ref-frame.  There are
  // several options for doing this; these could be enumerated and replaced
  // with a switch statement.
  switch (populate_rot_init_transform_source) {
   case RITS_None: // already fully populated
    break;
   case RITS_Inertial: // reference is inertial
    jeod::Matrix3x3::identity( T_inrtl_reference );
    break;

   case RITS_VelPos: // Velocity and position:
    if (requires_free_stream_velocity) {
      // 1st vector (velocity) specifies x-axis,
      // 2nd vector (position) provides -z-axis (general direction)
      // y-axis complets.
      MathUtils::generate_inrtl_to_reference ( free_stream_velocity,
                                               trans_init.position,
                                               T_inrtl_reference);
    }
    else {
      MathUtils::generate_inrtl_to_reference ( trans_init.velocity,
                                               trans_init.position,
                                               T_inrtl_reference);
    }
    break;
   case RITS_PfixRef: // Reference is known relative to Pfix
    jeod::Matrix3x3::product( T_pfix_reference,
                        planet->pfix.state.rot.T_parent_this,
                        T_inrtl_reference);
    break;
   case RITS_SolarPosPos:
   case RITS_SolarPosEclipticPole:
    // Can't get vehicle-sun vector directly because vehicle not yet
    // initialized.  Have to go through the planet.
    // First, find the sun-inertial frame, subscribe to it, and update
    // ephemerides to get its state  (and the state of planet-inertial).
    // Note - update-ephemerides usually gets run AFTER body initializations.

    // find_ref_frame is a DynBodyInit method, accessible by inheritance
    jeod::RefFrame * sun_inrtl = find_ref_frame( dyn_manager,
                                           "Sun.inertial",
                                           "ref-frame name");
    // NULL checked in find_ref_frame
    dyn_manager.subscribe_to_frame(*sun_inrtl);
    dyn_manager.update_ephemerides();
    // Now generate the vehicle to sun position vector
    double sun_to_veh_pos[3];
    sun_inrtl->compute_position_from( planet->inertial,
                                      sun_to_veh_pos );
    // Assume trans_init is expressed in inertial.  This was verified in
    // select_attitude_initializer
    jeod::Vector3::decr( trans_init.position,
                   sun_to_veh_pos);
    jeod::Vector3::negate(sun_to_veh_pos);
    /* Use this as the x-axis to generate the reference ref-frame orientation.
       NOTE - this is not the vehicle orientation; we have Euler angles to
       apply to the reference-ref-frame orientation to get vehicle
       orientation.
     The y-axis of the reference ref-frame is determined by a reference
     direction (y = x cross ref-dir, then z = x cross y is generally away from
     ref-dir).  That reference direction differs
     between these two options. Use the following for the reference direction
     - RITS_SolarPosPos:
         the position of the vehicle with respect to the reference planet
     - RITS_SolarPosEclipticPole:
         the the ecliptic south-pole, which is at a constant position in
         J2000, determined by the J2000 obliquity, 23.43929(1111) degrees.*/
    double ref_dir[3] = {0.0, 0.397777155, -0.917482062}; // south pole
    if (populate_rot_init_transform_source == RITS_SolarPosPos) {
      jeod::Vector3::copy (trans_init.position, ref_dir);
    }
    MathUtils::generate_inrtl_to_reference ( sun_to_veh_pos,
                                             ref_dir,
                                             T_inrtl_reference);

    // Unsubscribe from the sun-inertial frame.  No longer needed here.
    dyn_manager.unsubscribe_to_frame(*sun_inrtl);
    break;
  }

  // Applicable to RITS_SolarPosPos and RITS_VelPos.
  // Default puts the z-axis "down", with y-axis along (x-axis cross pos)
  // (negative angular momentum for case where x-axis defined by velocity).
  // If it is desired for z-axis to be "up" (e.g. b/c windows at -z should
  // be "down"), need to negate y- and z- axes.
  if (z_axis_points_up) {
    for (unsigned int ii = 1; ii < 3; ii++) {
      for (unsigned int jj = 0; jj < 3; jj++) {
        T_inrtl_reference[ii][jj] *= -1.0;
      }
    }
  }

  if (populate_rot_init_transform_source != RITS_None) {
    generate_rot_init_transform();
  }

  // Attitude rate:
  if (increment_rot_rate_init_with_pfix) {
    double ang_vel_planet_wrt_inrtl_in_inrtl[3];
    jeod::Vector3::transform_transpose( planet->pfix.state.rot.T_parent_this,
                                  planet->pfix.state.rot.ang_vel_this,
                                  ang_vel_planet_wrt_inrtl_in_inrtl);
    jeod::Vector3::transform_incr( rot_init.orientation.trans, // inertial to body
                             ang_vel_planet_wrt_inrtl_in_inrtl,
                             rot_init.ang_velocity);
  }
}




/*****************************************************************************
generate_pfix_ref_point_state
Purpose:(Generates a cartesian point state based off the specified alt/lat/lon
         values for the reference point.)
*****************************************************************************/
void
StateInitialize::generate_pfix_ref_point_state()
{
  pfix_ref_point_state.planet = planet;
  pfix_ref_point.altitude = ref_point_altitude;
  pfix_ref_point.latitude = ref_point_latitude;
  pfix_ref_point.longitude = ref_point_longitude;


  switch (pfix_ref_point_state.altlatlong_type) {
   case jeod::NorthEastDown::spherical:
     pfix_ref_point_state.update_from_spher (pfix_ref_point);
     break;

   case jeod::NorthEastDown::elliptical:
     pfix_ref_point_state.update_from_ellip (pfix_ref_point);
     break;

   default:
     CMLMessage::fail (
        __FILE__, __LINE__, jeod::BodyActionMessages::illegal_value, "\n",
         action_identifier, " failed:\n"
        "AltLatLong type option not recognized");
  }
  // Now have the position of the reference point relative to
  // ECEF (planet-center), expressed in:
  // cartesian coordinates : pfix_ref_point_state.ned_frame.state.trans.position
  // cartesian coordinates : pfix_ref_point_state.cart_coords[3]
  // spherical coordinates : pfix_ref_point_state.sphere_coords.
  //                                              <altitude,latitude,longitude>
  // elliptical coordinates: pfix_ref_point_state.ellip_coords.
  //                                              <altitude,latitude,longitude>


  // Generate the transformation from NED to pfix so that the position and
  // velocity values can be converted from being expressed in NED to
  // being expressed in PFIX (e.g. ECEF)

  pfix_ref_point_state.build_ned_orientation();
  // produces pfix_ref_point_state.ned_frame.state.rot.T_parent_this

  pfix_ref_point_state_generated = true;
}


/*****************************************************************************
generate_pfix_position
Purpose:(Generates a pfix cartesian position of the vehicle from a reference
         point and NED-expressed position relative to that reference point)
*****************************************************************************/
void
StateInitialize::generate_pfix_position()
{
  if (!pfix_ref_point_state_generated) {
    generate_pfix_ref_point_state();
  }

  jeod::Vector3::transform_transpose(
                  pfix_ref_point_state.ned_frame.state.rot.T_parent_this,
                  position,
                  pfix_position);
  jeod::Vector3::incr(pfix_ref_point_state.cart_coords,
                pfix_position);
  populate_trans_init_pos_from_pfix = true;
}


/*****************************************************************************
generate_pfix_velocity
Purpose:(Generates a pfix cartesian velocity of the vehicle from a reference
         point and NED-expressed velocity relative to that reference point)
*****************************************************************************/
void
StateInitialize::generate_pfix_velocity()
{
  if (use_veh_position_as_reference) {
    return;
  }

  // Otherwise, use the defined reference point as the NED origin.
  if (!pfix_ref_point_state_generated) {
    generate_pfix_ref_point_state();
  }

  jeod::Vector3::transform_transpose(
                  pfix_ref_point_state.ned_frame.state.rot.T_parent_this,
                  velocity,
                  pfix_velocity);
  populate_trans_init_vel_from_pfix = true;
}


/*****************************************************************************
generate_pfix_velocity_from_veh_pos
Purpose:(Generates the pfix velocity once the vehicle position is known)
*****************************************************************************/
void
StateInitialize::generate_pfix_velocity_from_veh_pos()
{
  pfix_ref_point_state.planet = planet;
  // Calculate cartesian position in the pfix frame
  jeod::Vector3::transform( planet->pfix.state.rot.T_parent_this,
                      trans_init.position,
                      pfix_ref_point_state.cart_coords);

  // Compute velocity from target relative parameters
  if (velocity_input_data_type==TR_InertialSpeedGammaLatang) {
    TR_param.compute_velocity_from_params( speed,
                                           flight_path_angle,
                                           TR_Lambda,
                                           pfix_ref_point_state.cart_coords,
                                           pfix_velocity);

  // Otherwise, compute from NED velocity
  } else {

    // Update other internal representations of pfix_ref_point_state
    pfix_ref_point_state.update_from_cart(pfix_ref_point_state.cart_coords);
    pfix_ref_point_state.build_ned_orientation();

    // vel_NED =   T_pfix->NED *  vel_pfix
    // T_NED->pfix * vel_NED    = vel_pfix
    jeod::Vector3::transform_transpose(
                  pfix_ref_point_state.ned_frame.state.rot.T_parent_this,
                  velocity,
                  pfix_velocity);
  }
  populate_trans_init_vel_from_pfix = true;
}

/*****************************************************************************
generate_free_stream_velocity
Purpose:(generates the velocity of the vehicle relative to the free stream,
         expressed in the inertial RF.)
Assumption:  (Input is the wind velocity relative to the planet expressed in
              inertial.)
*****************************************************************************/
void
StateInitialize::generate_free_stream_velocity(
      double rel_wind_vel_inrtl[3])
{
  if (rel_wind_vel_inrtl == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__, jeod::BodyActionMessages::illegal_value, "\n",
      "input vector is NULL.  Cannot process.\n"
      "Aborting free-stream velocity generation.\n");
    return;
  }

  // Start with the vehicle inertial velocity,
  //  - subtract off wxr to get the vehicle velocity relative to pfix expressed
  //    in inertial.
  //  - subtract off the (wind velocity relative to the planet expressed in
  //    inertial)
  // to get the (vehicle velocity relative to the wind expressed in inertial)
  double omega_inrtl[3];
  jeod::Vector3::transform_transpose( planet->pfix.state.rot.T_parent_this,
                                planet->pfix.state.rot.ang_vel_this,
                                omega_inrtl); //omega in inertial RF.
  jeod::Vector3::cross(omega_inrtl,
                 trans_init.position,
                 omega_cross_r); // in inertial
  jeod::Vector3::diff( trans_init.velocity,
                 omega_cross_r,
                 free_stream_velocity); // in inertial
  jeod::Vector3::decr( rel_wind_vel_inrtl,
                 free_stream_velocity); // in inertial
}

/*****************************************************************************
generate_rot_init_transform
Purpose:(Generates the transformation matrix from inertial to the reference
        frame)
*****************************************************************************/
void
StateInitialize::generate_rot_init_transform()
{
  rot_init.orientation.data_source = jeod::Orientation::InputMatrix;

  jeod::Orientation::compute_matrix_from_euler_angles( ref_body_sequence,
                                                 E_reference_body,
                                                 T_reference_body);
  jeod::Matrix3x3::product( T_reference_body,
                      T_inrtl_reference,
                      rot_init.orientation.trans);
}


/*****************************************************************************
generate_inertial_velocity
Purpose:(Generates a velocity-vector expressed in the inertial reference-frame
         from a velocity-vector expressed in the planet-fixed reference-frame.
         The planet-fixed velocity may be relative to either the inertial or
         planet-fixed frames.)
*****************************************************************************/
void
StateInitialize::generate_inertial_velocity()
{
  // We need velocity relative to inertial expressed in inertial.
  // In some cases, the velocity is relative to inertial and in some it is
  // relative to pfix.  In either, it is expressed in pfix, but we may
  // need to add in the pfix relative to inertial.
  if (!velocity_is_relative_to_inertial) {
    jeod::Vector3::cross(planet->pfix.state.rot.ang_vel_this,
                   pfix_ref_point_state.cart_coords,
                   omega_cross_r);
    jeod::Vector3::incr(omega_cross_r,
                  pfix_velocity);
  }

  jeod::Vector3::transform_transpose(planet->pfix.state.rot.T_parent_this,
                               pfix_velocity,
                               trans_init.velocity);
}

/*****************************************************************************
remove_from_trans_init
Purpose:(Removes one set of initialization options from the state intializer)
*****************************************************************************/
void
StateInitialize::remove_from_trans_init(
      jeod::DynBodyInitTransState::StateItems remove_item)
{
  if (trans_init.state_items == jeod::DynBodyInitTransState::Both) {
    if (remove_item == jeod::DynBodyInitTransState::Position) {
      trans_init.state_items = jeod::DynBodyInitTransState::Velocity;
    }
    else if (remove_item == jeod::DynBodyInitTransState::Velocity) {
      trans_init.state_items = jeod::DynBodyInitTransState::Position;
    }
  }
  else if (trans_init.state_items == remove_item) {
    use_trans_init = false;
  }
}

/*****************************************************************************
remove_from_rot_init
Purpose:(Removes one set of initialization options from the state intiializer)
*****************************************************************************/
void
StateInitialize::remove_from_rot_init(
      jeod::DynBodyInitRotState::StateItems remove_item)
{
  if (rot_init.state_items == jeod::DynBodyInitRotState::Both) {
    if (remove_item == jeod::DynBodyInitRotState::Attitude) {
      rot_init.state_items = jeod::DynBodyInitRotState::Rate;
    }
    else if (remove_item == jeod::DynBodyInitRotState::Rate) {
      rot_init.state_items = jeod::DynBodyInitRotState::Attitude;
    }
  }
  else if (rot_init.state_items == remove_item) {
    use_rot_init = false;
  }
}
/*****************************************************************************
generate_random
Purpose:(Generates a random unit vector and a random number)
Note -- see documentatation for CorrelatedStateDispersion for demonstration of
        why the individual axes should be distributed normally to generate a
        unit sphere with uniform distribution across the surface.
*****************************************************************************/
void
StateInitialize::generate_random(int seed)
{
  std::default_random_engine generator(seed);
  std::normal_distribution<double> rand_norm(0.0, 1.0);

  for (unsigned int ii = 0; ii < 3; ii++) {
    random_unit_vector[ii] = rand_norm(generator);
  }
  jeod::Vector3::normalize(random_unit_vector);

  std::uniform_real_distribution<double> rand_uniform(0.0, 1.0);
  random_value = rand_uniform( generator);
}

/*****************************************************************************
overwrite_vel_from_free_stream
Purpose:(Intended for post-application of this body action for cases that
         require the free-stream-velocity, hence the wind velocity, for
         computing the reference attitude.
         This method will re-generate the inertial-to-reference transformation
         matrix based on the free-stream velocity, rather than the pfix
         velocity.
         NOTE - this does not change the initialized velocity)
*****************************************************************************/
void
StateInitialize::overwrite_attitude_from_free_stream(
       double rel_wind_inrtl[3])
{
  if (!requires_free_stream_velocity) {
    CMLMessage::warn (
      __FILE__, __LINE__, jeod::BodyActionMessages::illegal_value, "\n",
      "overwrite_vel_from_free_stream was called, but this is only a valid\n"
      "call when the velocity was set for initialization with a free-stream"
      "value.\nIgnoring call");
    return;
  }

  if (rel_wind_inrtl == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__, jeod::BodyActionMessages::illegal_value, "\n",
      "input vector is NULL.  Cannot process.\n"
      "Aborting attitude-overwrite.\n");
    return;
  }

  generate_free_stream_velocity( rel_wind_inrtl );
  MathUtils::generate_inrtl_to_reference ( free_stream_velocity,
                                           trans_init.position,
                                           T_inrtl_reference);
  generate_rot_init_transform();
  // So now have the inrtial to body transformation matrix stuck in rot_init.
  // Need to apply it to the body
  // Cannot use apply() because this jeod::BodyAction has already been applied.
  // This method is for post-application only.

  auto* dyn_body = get_subject_dyn_body();
  if (dyn_body == nullptr) {
    CMLMessage::fail(
      __FILE__, __LINE__, "Invalid configuration\n",
      "The subject provided must be a valid DynBody.");
  }
  dyn_body->set_attitude_matrix(rot_init.orientation.trans,
                                dyn_body->composite_body);
}


/*****************************************************************************
verify_compatibility
Purpose:( Some options must be consistent across pos-vel and across
          att-att-rate.  This ensures that consistency)
*****************************************************************************/
void
StateInitialize::verify_compatibility()
{
  if ((force_match_trans &&
       (position_input_data_type != velocity_input_data_type))) {
    CMLMessage::fail (
      __FILE__, __LINE__,
      "Action ", action_identifier, " failed:\n"
      "Some options require similar usage between the partner initialize\n"
      "methods.\n"
      "For example, if using TransInit for position, then TransInit should\n"
      "also be used for velocity (or turn the velocity option off).\n"
      "Check the documentation for the requirements for the options that\n"
      "have been selected.\n"
      "Position: ", position_input_data_type, "\n"
      "Velocity: ", velocity_input_data_type, "\n");
  }

  if ((force_match_rot &&
       (attitude_input_data_type != att_rate_input_data_type))) {
    CMLMessage::fail (
      __FILE__, __LINE__,
      "Action ", action_identifier, " failed:\n"
      "Some options require similar usage between the partner initialize "
      "methods.\n"
      "For example, if using RotInit for attitude, then RotInit should\n"
      "also be used for attitude-rate (or turn it off).\n"
      "Check the documentation for the requirements for the options that "
      "have been selected.\n"
      "Attitude: ", attitude_input_data_type, " \n"
      "Att-Rate: ", att_rate_input_data_type, " \n");
  }
}
