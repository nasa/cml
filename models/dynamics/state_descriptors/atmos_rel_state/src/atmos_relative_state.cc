/********************************* TRICK HEADER *******************************
PURPOSE:
    (This  takes state information [position and velocity] and
     atmospheric information and computes atmospheric relative trajectory
     parameters and coordinate transformation matrices required by vehicle
     models.)

REFERENCE:
    (((SES II - atmos_relative_state_att function) (position_att.c))
     ((SES - Parameters of Motion function) (POM.f))
     ((SORT - Aero Functions) (AROVAR.f) (AROATT.f)))

LIBRARY DEPENDENCIES:
    ((utils/orientation/src/eigen_rotation.cc)
     (utils/orientation/src/euler_angles.cc)
     (utils/quaternion/src/quat_from_mat.cc)
     (utils/quaternion/src/quat_to_mat.cc)
    )

PROGRAMMERS:
    (((Gavin Mendeck) (NASA) (May 2005) (ARES)
          (Initial implementation as pom_pos))
     ((Edgar Medina) (NASA) (June 2005) (ARES)
          (Changed name and updated function))
     ((Jeremy Rea ) (NASA) (August 2005) (ARES)
          (Added total angle of attack computation))
     ((Pat Galvin ) (ESCG) (July 2006) (ARES)
          (RDLaa09011 - tag wind_velocity as ECI))
     ((Jeremy Rea) (NASA) (January 2007) (CEV)
          (RDLaa09504: Implement CEV aerodata subroutine V0.2 [density]))
     ((Jeremy Rea) (NASA) (01/08) (ANTARES)
          (RDLaa10259: Add Reynolds number computation))
     ((Debbie Merritt) (NASA) (07/08) (ANTARES)
          (Add qalpha, qbeta, qalpha_total calc))
     ((Gary Turner) (OSR) (Feb 15) (ANTARES)
          (Conversion to C++ and interface to planet-rel-state.))
     ((Bingquan Wang) (OSR) (Apr 17) (ANTARES)
          (Fixed the compilation warning of float-point number equality
           comparison))
    )
*******************************************************************************/

#define _USE_MATH_DEFINES // M_PI
#include <cmath>   // M_PI, isnan
#include <cstring> // NULL
#include "trick/constant.h" // conversions like RTD, LBFTON, MTF
#include "jeod/models/utils/orientation/include/orientation.hh" // Orientation
#include "jeod/models/utils/math/include/vector3.hh"    // Vector3
#include "jeod/models/utils/math/include/matrix3x3.hh"  // Matrix3x3
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "jeod/models/utils/quaternion/include/quat.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh" // MathUtils

#include "../include/atmos_relative_state.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
AtmosRelativeState::AtmosRelativeState(
      const jeod::DynBody       & body_ref,
      ExtendedPlanetaryDerivedState & planet_rel_state_ref,
      AtmosphereExecInterface       & atmos_exec_ref)
  :
  warn_on_v_parallel_r(true),
  terminate_on_nan_euler(false),

  free_stream_vel_mag(0.0),
  free_stream_vel(),
  free_stream_vel_unit(),
  free_stream_td_vel(),
  free_stream_body_vel(),

  free_stream_flight_path(0.0),
  free_stream_azimuth(0.0),
  angle_of_attack(0.0),
  angle_of_sideslip(0.0),
  bank_angle(0.0),
  total_angle_of_attack(0.0),
  phi_roll(0.0),

  mach_number(0.0),
  dynamic_pressure(0.0),
  drag_accel(0.0),
  side_accel(0.0),
  lift_accel(0.0),
  sensed_accel_mag(0.0),

  angle_of_attack_alt(0.0),
  angle_of_sideslip_alt(0.0),
  total_angle_of_attack_alt(0.0),
  phi_roll_alt(0.0),

  T_inrtl_traj(),
  T_traj_body(),
  T_traj_wind(),
  T_inrtl_wind(),
  T_wind_stab(),
  T_inrtl_stab(),

  reynolds_number(0.0),

  qalpha_total(0.0),
  qalpha_total_psfdeg(0.0),
  qalpha(0.0),
  qbeta(0.0),

  body(body_ref),
  planet_rel_state(planet_rel_state_ref),
  atmos_exec(atmos_exec_ref),
  atmos(atmos_exec.out),
  reference_length(NULL),
  compute_angles_subscriptions(0),
  complete_calculation_subscriptions(0),
  ref_length_null_msg_sent(false),
  traj_to_body_RYP(),
  psfdeg_converter(1.19664723485755)
{
  subscribe_name = "AtmosRelativeState:";
  jeod::Matrix3x3::identity(T_inrtl_traj);
  jeod::Matrix3x3::identity(T_inrtl_wind);
  jeod::Matrix3x3::identity(T_inrtl_stab);
  jeod::Vector3::initialize(traj_to_body_RYP);
}

/*****************************************************************************
activate
Purpose:(Sets the model active)
*****************************************************************************/
void
AtmosRelativeState::activate()
{
  // if already active, get out of here.
  // NOTE - this should not get hit, activate() is protected and only called
  // if !active.
  if (active) {
    return;
  }

  if (complete_calculation_subscriptions > 0 &&
      reference_length == NULL) {
    CMLMessage::warn(
      __FILE__, __LINE__, "Atmosphere Relative State Setup error\n",
      "The flag to perform the complete calculation has been set but\n"
      "the reference-length pointer has not been set.\n"
      "The reference length pointer is required for the computation of "
      "the Reynolds number.\n"
      "Continuing, but Reynolds number will not be evaluated.\n");
  }

  // We are about to subscribe to atmos-exec, which subscribes to
  // planet-rel-state.
  // If atmos-exec already subscribed, then so is planet-rel-state; this will
  //   just add 1 to the atmos-exec subscription count.
  // If atmos-exec is not already subscribed but planet-rel-state is, this will
  //   activate atmos-exec, which adds a subscription to planet-rel-state,
  //   and re-runs planet-rel-state (to make it current with the needs of
  //   atmos-exec)
  // If neither is already subscribed, this will activate both, which runs
  //   planet-rel-state (but not atmos-exec).

  // In addition to the demands that atmos-exec places on planet-rel, this
  // model requires planet-rel-state to compute the relative velocities (a
  // step not required by atmos_exec).

  // In the case that planet-rel-state is not going to be run/rerun
  // automatically with the subscription to atmos-exec, then if its
  // relative-velocity computation is not currently active, we will have to
  // force a rerun of planet-rel-state after making its rel-vel component
  // active.
  // Planet-rel-state will run/rerun automatically if atmos-exec is not
  // currently active, but not otherwise.  So we need to rerun
  // planet-rel if (and only if) atmos-exec is already active AND no other
  // model already subscribes to the relative-velocity computation.
  bool rerun_planet_rel_state = atmos_exec.is_active() &&
                                !planet_rel_state.is_rel_vel_subscribed();

  // Add a subscription to the relative-velocity computation
  planet_rel_state.subscribe_rel_vel();

  // Now subscribe to atmos_exec, which subscribes to planet-rel-state.
  atmos_exec.subscribe();

  if (rerun_planet_rel_state) {
    planet_rel_state.update();
  }

  // And run this model
  active = true;
  update();
}

/*****************************************************************************
deactivate
Purpose:(Sets the model inactive)
*****************************************************************************/
void
AtmosRelativeState::deactivate()
{
  if (active) {
    planet_rel_state.unsubscribe_rel_vel();
    atmos_exec.unsubscribe();
    active = false;
  }
}

/*****************************************************************************
subscribe and unsubscribe methods
Purpose:Sets the model to (not) perform the angles and full calculation set
*****************************************************************************/
void
AtmosRelativeState::subscribe_complete_calculation()
{
  subscribe_angles_calculation();
  complete_calculation_subscriptions++;
}
//****************************************************************************
void
AtmosRelativeState::unsubscribe_complete_calculation()
{
  if (complete_calculation_subscriptions > 0) {
    unsubscribe_angles_calculation();
    complete_calculation_subscriptions--;
  }
}
//****************************************************************************
void
AtmosRelativeState::subscribe_angles_calculation()
{
  compute_angles_subscriptions++;
}
//****************************************************************************
void
AtmosRelativeState::unsubscribe_angles_calculation()
{
  if (compute_angles_subscriptions > 0) {
    compute_angles_subscriptions--;
  }
}

/*******************************************************************************
Method: set_reference_length
Purpose: Sets the reference length pointer
*******************************************************************************/
void
AtmosRelativeState::set_reference_length(
   double * ref_length_in)
{
  reference_length = ref_length_in;
  initialize();
}

/*******************************************************************************
Method: update
Purpose: (This function takes state information [position and velocity] and
     atmospheric information and computes atmospheric relative trajectory
     parameters and coordinate transformation matrices required by vehicle
     models.)
*******************************************************************************/
void
AtmosRelativeState::update()
{
  if (!active) {
    return;
  }
  // Compute free stream velocity, Mach number, and dynamic pressure.
  jeod::Vector3::diff( planet_rel_state.relative_vel,
                 atmos.wind_velocity_eci,
                 free_stream_vel);

  free_stream_vel_mag = jeod::Vector3::vmag( free_stream_vel );

  if (MathUtils::is_near_equal(free_stream_vel_mag, 0.0)) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Free stream velocity vanished to 0.0\n",
      "Atmosphere relative state is not well defined because the vehicle has\n"
      "no velocity relative to the atmosphere.\n");
    update_zero_vel();
    return;
  }

  jeod::Vector3::scale( free_stream_vel,
                  MathUtils::divide_protected(1.0, free_stream_vel_mag, 0.0),
                  free_stream_vel_unit);

  mach_number = MathUtils::divide_protected(free_stream_vel_mag,
                                            atmos.speed_of_sound,
                                            0.0);

  dynamic_pressure = 0.5 * atmos.density
                         * free_stream_vel_mag * free_stream_vel_mag;

  if (compute_angles_subscriptions > 0) {
    compute_angles();
  }

  if (complete_calculation_subscriptions > 0) {
    // Compute the free stream flight path angle and azimuth.
    compute_fpangle_azimuth();

    // Compute the inertial to wind coordinate transformation and .
    //         the inertial to stability coordinate transformation.
    compute_wind_stability_transforms();

    // Compute accelerations in the wind frame.
    // (requires compute_wind_stability_transforms() )
    compute_accel_wind();

    // Make alternate computations of the aerodynamic angles.
    compute_alternate_angles();

    // compute_dynamic-pressure additional variables
    compute_dynamic_pressure_extras();

    // compute Reynold Number
    compute_reynolds_number();
  }
}

/*****************************************************************************
update_zero_vel
Purpose:(Assigns nominal values when the vehicle has zero velocity relative
         to the atmosphere.)
*****************************************************************************/
void
AtmosRelativeState::update_zero_vel()
{
  jeod::Vector3::unit(0,free_stream_vel_unit);
  mach_number = 0.0;
  dynamic_pressure = 0.0;
  if (compute_angles_subscriptions > 0) {
    bank_angle = 0.0;
    angle_of_sideslip = 0.0;
    angle_of_attack = 0.0;
    total_angle_of_attack = 0.0;
    phi_roll = 0.0;
  }
  if (complete_calculation_subscriptions > 0) {
    free_stream_flight_path = 0.0;
    free_stream_azimuth = 0.0;
    // Hold T_inrtl_* fixed at current values and recompute accelerations.
    compute_accel_wind();
    total_angle_of_attack_alt =
    angle_of_attack_alt       =
    angle_of_sideslip_alt     =
    phi_roll_alt =
    qalpha_total =
    qalpha_total_psfdeg =
    qalpha =
    qbeta =
    reynolds_number = 0.0;
  }
}

/*****************************************************************************
compute_angles
Purpose:(Computes the flight-path-body angles)
*****************************************************************************/
void
AtmosRelativeState::compute_angles()
{
  // Compute the transformations from trajectory to body frame.
  compute_T_inrtl_traj();

  // Combine T_{inrtl->body} and T_{inrtl->traj} to get T_{traj->body}
  jeod::Matrix3x3::product_right_transpose(
                        body.composite_body.state.rot.T_parent_this,
                        T_inrtl_traj,
                        T_traj_body);

  // Extract the Euler angles from the trajectory to body matrix.
  jeod::Orientation::compute_euler_angles_from_matrix( T_traj_body,
                                                 jeod::Orientation::Roll_Yaw_Pitch,
                                                 traj_to_body_RYP);

  // NOTE - if T_traj_body got really badly corrupted, this can produce NaN
  // output.  Check for that before continuing.
  // Aside - T_traj_body has got to be REALLY badly corrupted for NaN to creep
  //         in from "compute_euler_angles_from_matrix"; a numerical glitch
  //         on a few components of T_traj_body will pass through just fine.
  //               badly corrupted:               manageable:
  //             [ 1.1  ,  0.0  , 0.0]         [ 0.0  ,  1.1  , 0.0]
  //             [ 1.1  ,  1.1  , 1.1]         [-1.1  ,  0.0  , 0.0]
  //             [ 1.1  ,  0.0  , 0.0]         [ 0.0  ,  0.0  , 1.1]
  //         With the protections that are already in place on the generation
  //         of T_traj_body, it may be really difficult to find a situation
  //         in which this level of corruption is achievable.
  check_euler_for_nan();

  bank_angle = traj_to_body_RYP[0];
  angle_of_sideslip = -traj_to_body_RYP[1];
  angle_of_attack = traj_to_body_RYP[2];
  total_angle_of_attack = MathUtils::acos_protected(
                                     std::cos(angle_of_attack) *
                                     std::cos(angle_of_sideslip));
  phi_roll = std::atan2( std::tan(angle_of_sideslip),
                         std::sin(angle_of_attack));
}


/*******************************************************************************
Method: compute_T_inrtl_traj
Purpose: Computes the transformation matrix from inertial to trajectory.
         Trajectory is oriented with:
           X- along the velocity vector,
           Y along the negative angular momentum vector, and
           Z completing (often close to -r).
*******************************************************************************/
void
AtmosRelativeState::compute_T_inrtl_traj()
{
  // Angular momentum = r x v; compute -Angular-momentum = v x r for the
  // y-axis
  double y_axis[3];
  jeod::Vector3::cross( free_stream_vel,
                  body.composite_body.state.trans.position,
                  y_axis); // need to normalize

  // Check for vel and pos being parallel or anti-parallel
  // If so, reference frame undefined.  Retain old value.
  // Notes:
  //   - T_inrtl_traj is a class member, so returning at this point will
  //     leave it untouched.
  //   - Applying after the normalization to avoid any comparison differences
  //     that might result in the magnitude of the pre-normalized scratch
  //     passing the "is-near-equal" test, and then failing the equivalent
  //     test at normalization ... which would result in a [0,0,0] vector for
  //     the y-axis.
  double y_axis_mag = jeod::Vector3::vmag(y_axis);
  if (MathUtils::is_near_equal( y_axis_mag, 0.0)) {
    if (std::isnan(y_axis[0])) { // check for NaN, vmag turns NaN into 0.
      CMLMessage::error(
        __FILE__,__LINE__,"Reference Frame Error.\n",
        "The y-axis of the trajectory frame has NaN content."
        "Holding previous definition of trajectory frame.\n\n"
        "Reference values:\n"
        "free_stream_vel: [", free_stream_vel[0], " ", free_stream_vel[1], " ", free_stream_vel[2], "]\n"
        "position:        [", body.composite_body.state.trans.position[0], " ", body.composite_body.state.trans.position[1], " ", body.composite_body.state.trans.position[2], "]\n"
        "y-axis:          [", y_axis[0], " ", y_axis[1], " ", y_axis[2], "]");
    }
    else if ( warn_on_v_parallel_r) {
      jeod::Vector3::normalize( y_axis); // returns [0,0,0] if y_axis = [0,0,0] or NaN
      CMLMessage::warn(
        __FILE__,__LINE__,"Reference frame undefined.\n",
        "The free-stream velocity and position vectors are parallel\n"
        "(or anti-parallel).  The trajectory-reference-frame is undefined.\n"
        "Holding previous definition of the trajectory-reference-frame.\n\n"
        "Reference values:\n"
        "free_stream_vel: [", free_stream_vel[0], " ", free_stream_vel[1], " ", free_stream_vel[2], "]\n"
        "position:        [", body.composite_body.state.trans.position[0], " ", body.composite_body.state.trans.position[1], " ", body.composite_body.state.trans.position[2], "]\n"
        "y-axis:          [", y_axis[0], " ", y_axis[1], " ", y_axis[2], "]");
    }
    return;
  }
  jeod::Vector3::normalize( y_axis); // returns [0,0,0] if y_axis = [0,0,0] or NaN

  // Note -- mathematically, we could go straight to z-axis with the cross
  // product without need normalization; the y-axis is perpendicular to the
  // free-stream vector, and both must have magnitude 1.0 at this point, so
  // the cross product should also have magnitude 1.0.
  // But better to be safe and avoid any numerical imprecisions that might
  // come up.
  double z_axis[3];
  jeod::Vector3::cross( free_stream_vel_unit,
                  y_axis,
                  z_axis);
  jeod::Vector3::normalize( z_axis); // returns [0,0,0] if z_axis = [0,0,0] or NaN
  // Check again, just for rigor.  This check should be impossible to fail.
  if (MathUtils::is_near_equal( jeod::Vector3::vmag(z_axis), 0.0)) {
    CMLMessage::error(
      __FILE__,__LINE__,"Reference frame undefined.\n",
      "Unknown error resulted in an undefined trajectory reference frame\n"
      "This error message should never be seen\n"
      "Holding previous definition of this frame.\n");
    return;
  }

  // Use these 3 vectors to construct the transformation matrix
  for( unsigned int ii = 0 ; ii < 3 ; ii++ ) {
    T_inrtl_traj[0][ii] = free_stream_vel_unit[ii];
    T_inrtl_traj[1][ii] = y_axis[ii];
    T_inrtl_traj[2][ii] = z_axis[ii];
  }
}

/*****************************************************************************
compute_fpangle_azimuth
Purpose: Compute the free stream flight path angle and azimuth.
*****************************************************************************/
void
AtmosRelativeState::compute_fpangle_azimuth()
{
  jeod::Vector3::transform( planet_rel_state.topodetic.T_inrtl_to_this,
                      free_stream_vel,
                      free_stream_td_vel);

  double free_stream_td_vel_xy = std::sqrt(
                            free_stream_td_vel[0] * free_stream_td_vel[0] +
                            free_stream_td_vel[1] * free_stream_td_vel[1] );

  free_stream_flight_path = std::atan2( -free_stream_td_vel[2] ,
                                         free_stream_td_vel_xy );
  free_stream_azimuth = std::atan2( free_stream_td_vel[1],
                                    free_stream_td_vel[0] );
}

/*****************************************************************************
compute_wind_stability_transforms
Purpose: Compute the inertial to wind coordinate transformation and .
                 the inertial to stability coordinate transformation.
*****************************************************************************/
void
AtmosRelativeState::compute_wind_stability_transforms()
{
  double eigen_axis[3];
  jeod::Vector3::unit(0, eigen_axis); // [1,0,0]
  jeod::Orientation::compute_matrix_from_eigen_rotation( bank_angle,
                                                   eigen_axis,
                                                   T_traj_wind);
  jeod::Matrix3x3::product( T_traj_wind,
                      T_inrtl_traj,
                      T_inrtl_wind);

  // Create the inertial to stability coordinate transformation.
  jeod::Vector3::unit(2, eigen_axis); // [0,0,1]
  jeod::Orientation::compute_matrix_from_eigen_rotation( -angle_of_sideslip,
                                                   eigen_axis,
                                                   T_wind_stab);
  jeod::Matrix3x3::product( T_wind_stab,
                      T_inrtl_wind,
                      T_inrtl_stab);
}

/*****************************************************************************
compute_accel_wind
Purpose: Compute accelerations in the wind frame.
Assumption: requires prior execution of compute_wind_stability_transforms()
*****************************************************************************/
void
AtmosRelativeState::compute_accel_wind()
{
  // Compute accelerations in the wind frame.
  double wind_accel[3]; // for generating drag_accel, etc. only
  jeod::Vector3::transform( T_inrtl_wind,
                      body.derivs.non_grav_accel,
                      wind_accel);
  drag_accel = -wind_accel[0];
  side_accel =  wind_accel[1];
  lift_accel = -wind_accel[2];
  sensed_accel_mag = jeod::Vector3::vmag( body.derivs.non_grav_accel );
}

/*****************************************************************************
compute_alternate_angles
Purpose: Alternate computations of the aerodynamic angles.
*****************************************************************************/
void
AtmosRelativeState::compute_alternate_angles()
{
  // Transform wind relative velocity to the body frame.
  jeod::Vector3::transform( body.composite_body.state.rot.T_parent_this,
                      free_stream_vel,
                      free_stream_body_vel);

  double free_stream_body_vel_xz = std::sqrt( // for angle_of_sideslip_alt only
                            free_stream_body_vel[0] * free_stream_body_vel[0] +
                            free_stream_body_vel[2] * free_stream_body_vel[2]);

  double free_stream_body_vel_yz = std::sqrt( // for tot_ang_attack only
                            free_stream_body_vel[1] * free_stream_body_vel[1] +
                            free_stream_body_vel[2] * free_stream_body_vel[2]);

  total_angle_of_attack_alt = std::atan2( free_stream_body_vel_yz,
                                          free_stream_body_vel[0] );
  angle_of_attack_alt = std::atan2( free_stream_body_vel[2],
                                    free_stream_body_vel[0] );
  angle_of_sideslip_alt = std::atan2( free_stream_body_vel[1],
                                      free_stream_body_vel_xz );
  phi_roll_alt = std::atan2( free_stream_body_vel[1],
                             free_stream_body_vel[2] );
}

/*****************************************************************************
compute_dynamic_pressure_extras
Purpose: compute_dynamic-pressure additional variables
*****************************************************************************/
void
AtmosRelativeState::compute_dynamic_pressure_extras()
{
  // Q_alpha (and Q_alpha_total) should be proportional to alpha between
  // alpha = 0 to alpha = pi/2 and be symmetric about alpha = pi/2.
  // Q_alpha = { P_dyn alpha             0 <= alpha <= pi/2
  //           { P_dyn (pi - alpha)    pi/2 < alpha <= pi
  qalpha = dynamic_pressure * (M_PI_2 - std::abs(angle_of_attack - M_PI_2));
  qalpha_total = dynamic_pressure *
                 (M_PI_2 - std::abs(total_angle_of_attack - M_PI_2));
  // Convert to traditional units:
  qalpha_total_psfdeg = qalpha_total * psfdeg_converter;
  // NOTE: this previously used Trick's conversions at constant.h:
  //       qalpha_total_psfdeg = qalpha_total * RTD / LBFTON / MTF / MTF;
  //       However, Trick's MTF value is rounded in the 6th sig fig; it would
  //       have been better to use qalpha_total * RTD / LBFTON * FTOM * FTOM
  //       with FTOM being exact (by definition).

  qbeta        = dynamic_pressure * angle_of_sideslip;
}

/*****************************************************************************
compute_reynolds_number
Purpose: compute Reynold Number
*****************************************************************************/
void
AtmosRelativeState::compute_reynolds_number()
{
  if (reference_length != NULL) {
    reynolds_number = MathUtils::divide_protected(
                        atmos.density*free_stream_vel_mag*(*reference_length),
                        atmos.dynamic_viscosity,
                        reynolds_number);
  }
  else if (!ref_length_null_msg_sent) {
    CMLMessage::error(
      __FILE__, __LINE__, "Atmosphere Relative State Setup error\n",
      "Computation of Reynolds Number requires a reference length, which "
      "has not been assigned.\n"
      "Reynolds number for vehicle ", body.name, " will not be computed.\n"
      "Use method AtmosRelativeState::set_reference_length( double *)\n"
      "to set this pointer.\n"
      "This message will only be sent once.\n");
    ref_length_null_msg_sent = true;
  }
  // else - no action; cannot generate Re and message has already been sent.
}

/*****************************************************************************
check_euler_for_nan
Purpose: Checks the euler-angles generated from a transformation matrix for
         any presence of NaN which would indicate a corrupted transformation
         matrix.  If any found, it attempts to normalize the matrix and
         regenerate the euler angles.
*****************************************************************************/
void
AtmosRelativeState::check_euler_for_nan()
{
  for (unsigned int ii = 0; ii < 3; ii++) {
    // Check for NaN values
    if ( std::isnan(traj_to_body_RYP[ii])) {
      CMLMessage::error( __FILE__,__LINE__,
        "Corrupted traj-to-body transformation matrix.\n"
        "The RYP Euler angles corresponding to T_traj_body contain a NaN\n"
        "at index ", ii, ".  Attempting fix.\n");

      // NAN DETECTED!
      // Record the original transformation matrix for error reporting.
      double T_original[3][3];
      jeod::Matrix3x3::copy( T_traj_body,
                       T_original);

      // Now normalize it and recompute the euler angles.
      normalize_T_traj_body();
      jeod::Orientation::compute_euler_angles_from_matrix( T_traj_body,
                                               jeod::Orientation::Roll_Yaw_Pitch,
                                               traj_to_body_RYP);

      // recheck for NaN (the normalization may have failed as well).
      // Note - could make this method iterate on itself, but there is at
      //        most one follow-on iteration so this is easier than adding
      //        infinite-loop protection
      for (unsigned int jj = 0; jj < 3; jj++) {
        if ( std::isnan(traj_to_body_RYP[jj])) {
          // Still got a NAN in there.  Nothing we can do to fix it at this
          // point.  Got 2 options for bailing out:
          if (terminate_on_nan_euler) {
            CMLMessage::fail(
              __FILE__,__LINE__,"Corrupted traj-to-body transformation matrix.\n",
              "The T_traj_body transformation matrix was badly corrupted.\n"
              "Renormalization was attempted but it failed.\n"
              "The trajectory frame appears to be ill-defined.\n"
              "Terminating sim per setting of 'terminate_on_nan_euler = true'."
              "\n");
            // exit - terminated
          }
          else {
            CMLMessage::error(
              __FILE__,__LINE__,"Corrupted traj-to-body transformation matrix.\n",
              "The T_traj_body transformation matrix was badly corrupted.\n"
              "Renormalization was attempted but it failed.\n"
              "The trajectory frame appears to be ill-defined.\n"
              "Setting Euler angles to [0,0,0] and continuing per setting\n"
              "of 'terminate_on_nan_euler = false'.\n"
              "Reference values:\n"
              "original trajectory-to-body:\n"
              "[", T_original[0][0], "  ", T_original[0][1], "  ", T_original[0][2], "]\n"
              "[", T_original[1][0], "  ", T_original[1][1], "  ", T_original[1][2], "]\n"
              "[", T_original[2][0], "  ", T_original[2][1], "  ", T_original[2][2], "]\n");
            jeod::Vector3::initialize(traj_to_body_RYP);
            return;
          }
        }
      }

      // all values are non-NaN at this point.  Normalization worked.
      CMLMessage::error(
        __FILE__,__LINE__,"Corrupted traj-to-body transformation matrix.\n",
        "The T_traj_body transformation matrix was badly corrupted.\n"
        "The matrix was succesfully normalized from:\n"
        "[", T_original[0][0], "  ", T_original[0][1], "  ", T_original[0][2], "]\n"
        "[", T_original[1][0], "  ", T_original[1][1], "  ", T_original[1][2], "]\n"
        "[", T_original[2][0], "  ", T_original[2][1], "  ", T_original[2][2], "]\n"
        "to:\n"
        "[", T_traj_body[0][0], "  ", T_traj_body[0][1], "  ", T_traj_body[0][2], "]\n"
        "[", T_traj_body[1][0], "  ", T_traj_body[1][1], "  ", T_traj_body[1][2], "]\n"
        "[", T_traj_body[2][0], "  ", T_traj_body[2][1], "  ", T_traj_body[2][2], "]\n");

      // break out of the for-loop with a detection on any axis.
      // At this point, the matrix has been normalized following the
      // original detection and there is no point in continuing to check the
      // other axes because they are no longer valid -- the matrix values used
      // to generate these old euler angles have been overwritten.
      break;
    }
  }
}

/*****************************************************************************
normalize_T_traj_body
Purpose: Normalizes the transformation matrix in the event that it is not
         possible to obtain euler angles from it.
*****************************************************************************/
void
AtmosRelativeState::normalize_T_traj_body()
{
  // Convert to quaternion, normalize and convert back.
  // Note - a bit redundant because the euler angles could have been generated
  // from teh normalized quaternion without going back through the
  // transformation matrix.  But that would leave the matrix disconnected from
  // its euler angles, which could be confusing.
  jeod::Quaternion Q_traj_body;
  Q_traj_body.left_quat_from_transformation( T_traj_body);
  Q_traj_body.normalize();
  Q_traj_body.left_quat_to_transformation( T_traj_body);
}
