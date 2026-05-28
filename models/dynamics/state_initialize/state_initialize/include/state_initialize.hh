/*******************************************************************************

PURPOSE:
   (Provide the capability to initialize the state from a number of potential
   sources.  This class inherits from the JEOD jeod::BodyAction class and combines
   the capabilities of JEOD's DynBodyInitState with CML's ORBITAL_ELEMENTS,
   ORB_INIT, and ICOPT models, and CEV's Corr_State_Init_T and
   State_Init_Exec_T structures and the state_init_exec method.)

LIBRARY DEPENDENCIES:
   ((../src/state_initialize.cc)
   )

PROGRAMMERS:
   (((Gary Turner) (OSR) (September 2014) (New))
 ******************************************************************************/

#ifndef ANTARES_STATE_INITIALIZE_HH
#define ANTARES_STATE_INITIALIZE_HH

#include <string>

#include "jeod/models/dynamics/body_action/include/dyn_body_init.hh"
#include "jeod/models/dynamics/body_action/include/dyn_body_init_trans_state.hh"
#include "jeod/models/dynamics/body_action/include/dyn_body_init_rot_state.hh"
#include "jeod/models/dynamics/body_action/include/dyn_body_init_orbit.hh"
#include "jeod/models/dynamics/body_action/include/dyn_body_init_ned_rot_state.hh"
#include "jeod/models/dynamics/dyn_manager/include/dyn_manager.hh"
#include "jeod/models/environment/planet/include/planet.hh"
#include "jeod/models/utils/planet_fixed/north_east_down/include/north_east_down.hh"
#include "jeod/models/utils/orientation/include/orientation.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "cml/models/utilities/math_utils/include/math_utils.hh" // MathUtils::*
#include "cml/models/dynamics/state_initialize/correlated_state_dispersion/include/correlated_state_dispersion.hh"
#include "cml/models/dynamics/state_initialize/target_relative_parameters/include/TR_state_parameters.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "monte_carlo_state_dispersion.hh"

class StateInitialize : public jeod::DynBodyInit
{
 public:
  enum InputDataType {
    Unspecified,
    Inertial,
    Pfix,
    ICOPT_R_INERTIAL,
    ICOPT_V_INERTIAL,
    NED_Geodetic,
    GEOD_LAT_LON_ALT,        // deprecated
    NED_Geocentric,
    InertialSpeedGammaAzimuth,
    ICOPT_INERT_VEL_GAM_AZ,  // deprecated
    PfixSpeedGammaAzimuth,
    ICOPT_REL_VEL_GAM_AZ,    // deprecated
    OrbitalElements,
    EulerInertialVelvec_YPR,
    EulerPfixVelvec_YPR,     // deprecated
    SunPointing_YPR,
    SunPointingEcliptic_PYR,
    PfixUpCompAzimuth,       // deprecated
    PfixUpCompAzimuth_YPR,
    EulerNED_YPR,
    EulerBoostRef_YPR,
    ICOPT_EUL_YPR_BR,        // deprecated
    EulerPlumbline_PYR,
    ICOPT_EUL_PYR_PL,        // deprecated
    EulerFreestream_RYP,
    ICOPT_EUL_RYP_FS,        // deprecated
    MatrixInertialBody,
    ICOPT_INERT_BODY_ROTMAT, // deprecated
    EulerInertial_YPR,
    ICOPT_EUL_YPR_INERT_BODY,// deprecated
    BodyInertialRate,
    BodyPfixRate,
    Random,
    Off,
    TransInit,
    RotInit,
    RotInit_NED,
    TR_GeodAlt_RngAng_RotAng,
    TR_GeodAlt_RngAng_CrossAng_AbsRotAng_LT_90,
    TR_GeodAlt_RngAng_CrossAng_AbsRotAng_GT_90,
    TR_InertialSpeedGammaLatang
  };

  enum RotInitTransformSource {
    // RITS is an abbreviation of RotInitTransformSource
    RITS_None,
    RITS_Inertial,
    RITS_VelPos,
    RITS_PfixRef,
    RITS_SolarPosPos,
    RITS_SolarPosEclipticPole
  };

  InputDataType position_input_data_type; /* (--) enumeration for position */
  InputDataType velocity_input_data_type; /* (--) enumeration for velocity */
  InputDataType attitude_input_data_type; /* (--) enumeration for attitude */
  InputDataType att_rate_input_data_type; /* (--) enumeration for att rate */

  jeod::DynBodyInitOrbit         orb_elem_init; /* (--) orbital element initializer */
  jeod::DynBodyInitTransState    trans_init; /* (--) translational state initializer */
  jeod::DynBodyInitRotState      rot_init;   /* (--) rotational state initializer */
  jeod::DynBodyInitNedRotState   ned_rot_init; /* (--) rot state off trans state */

  CorrelatedStateDispersion  correlation; /* (--) correlated state disperser */
  MonteCarloStateDispersion  monte_carlo_dispersion; /* (--)
        random state disperser */

  // Input / Output
  TargetRelative_StateParameter TR_param; /* (--)
         Instance of model responsible for  converting between position/velocity
         and the TargetRelative parameter set.  Included as a class instance
         to facilitate population of target location parameters, which are
         class members.*/

  // Input values:
  std::string planet_name; /* (--) Name of the planet (typically "Earth") */

  // Reference point definition for translational state initialization
  double ref_point_altitude;  /* (m) The altitude of the reference point. */
  double ref_point_latitude;  /* (rad) The latitude of the reference point. */
  double ref_point_longitude; /* (rad) The longitude of the reference point. */

  // Target relative parameters for position initialization */
  double TR_geodetic_altitude;/* (m)  Geodetic altitude of target relative
                                      position */
  double TR_theta_Rng; /* (rad)  Position range angle from target             */
  double TR_theta_Rot; /* (rad)  Position rotation angle about target vector  */
  double TR_phi_Cross; /* (rad)  Position cross angle                         */

  // Reference point definition for rotational state initialization
  double rot_ref_latitude; /* (rad) latitude for rotational state initialization*/
  double rot_ref_longitude;/* (rad) longitude for rotational state
                                  initialization */
  double rot_ref_azimuth;  /* (rad) azimuth for rotational state initialization */

  // Velocity definition for speed / flight-path / azimuth option
  double speed;          /* (m/s) for speed/flight-path/azimuth
                                  and speed/flight-path/lateral-angle
                                  velocity options */
  double flight_path_angle;/* (rad) for speed/flight-path/azimuth
                                  and speed/flight-path/lateral-angle
                                  velocity options */
  double azimuth;          /* (rad) for speed/flight-path/azimuth
                                  velocity options */
  double TR_Lambda;        /* (rad) for speed/flight-path/lateral angle
                                  velocity option */

  // Attitude definition for yaw / pitch / roll option
  double yaw;    /* (rad) for yaw / pitch / roll attitude init option */
  double roll;   /* (rad) for yaw / pitch / roll attitude init option */
  double pitch;  /* (rad) for yaw / pitch / roll attitude init option */

  // Attitude defintion for bank / sideslip / attack option
  double angle_of_bank;     /* (rad) bank angle     for EulerFreestream_RYP attitude option */
  double angle_of_sideslip; /* (rad) sideslip angle for EulerFreestream_RYP attitude option */
  double angle_of_attack;   /* (rad) attack angle   for EulerFreestream_RYP attitude option */

  // Attitude defintion for matrix option
  double T_inrtl_body[3][3]; /* (--) Direct transform inertial to body. */

  // Euler sequence override
  jeod::Orientation::EulerSequence ref_body_sequence_override; /* (--)
         The RPY/PRY etc sequence IF it is desirable to override the
         default settings for the specified attitude type.*/

  // Attitude rate defintion for roll / pitch / yaw option
  double roll_rate_body;  /* (rad/s)
         used for roll/pitch/yaw attitude rate init option */
  double pitch_rate_body; /* (rad/s)
         used for roll/pitch/yaw attitude rate init option */
  double yaw_rate_body;   /* (rad/s)
         used for roll/pitch/yaw attitude rate init option */

  // Attitude rate specifications for random state
  double max_rate_random;  /* (rad/s) Max attitude rate for random rate option */
  int att_random_seed;     /* (--) seed for the random attitude generator */
  int att_rate_random_seed;/* (--) seed for the random attitude-rate generator */


  // Computed values that may be set or need to be accessed from outside:
  double free_stream_velocity[3]; /* (m/s)
         The velocity of the vehicle relative to the free stream, expressed in
         the inertial frame.*/


 protected:
  bool initialized; /* (--) Model is initialized, planet is non-NULL. */
  jeod::Planet * planet; /* (--) Pointer to the planet that defines the pfix frame. */

  // Reference frame names
  std::string reference_ref_frame_name_inertial; /* (--)
      Inertial reference frame name. */
  std::string reference_ref_frame_name_pfix; /* (--)
      Planet-fixed reference frame name. */

  // Flags to indicate which state initializers are defined and to be used.
  bool use_orbital_init; /* (--) flag to use orb_elem_init */
  bool use_trans_init; /* (--) flag to use trans_init */
  bool use_rot_init; /* (--) flag to use rot_init */
  bool use_ned_rot_init; /* (--) flag to use ned_rot_init */
  bool use_trans_init_passthrough; /* (--)
               flag to use trans_init without input from this model.*/
  bool use_rot_init_passthrough; /* (--)
               flag to use rot_init without input from this model.*/
  bool use_pfix_frame_trans; /* (--)
               flag to use initialize trans state using pfix frame instead of inertial.*/

  // Flags to indicate procedural completions
  bool pfix_ref_point_state_generated; /* (--)
          The pfix-cartesian representation of the reference point is
          available. */

  bool use_veh_position_as_reference; /* (--)
          For cases in which the velocity is defined relative to the NED frame
          associated with the vehicle position.*/

  bool populate_trans_init_pos_from_pfix; /* (--)
          The position may be initialized relative to some position that is
          defined in pfix.  pfix must be defined and computed in order to get
          the position in inertial, which is needed before the body action
          can be applied. So wait for the apply() method before populating
          trans)_init.position*/

  bool populate_trans_init_vel_from_pfix; /* (--)
          The velocity is initialized in pfix but pfix must be defined and
          computed in order to get the velocity in inertial, which is needed
          before the body action can be applied.*/

  bool velocity_is_relative_to_inertial; /* (--)
         Some init options provide an inertial-referenced velocity, and some
         provide a pfix/ned-referenced velocity.  This flag distinguishes the
         cases. */

  bool requires_free_stream_velocity; /* (--)
         For initializing the attitude relative to the free-stream.*/

  RotInitTransformSource populate_rot_init_transform_source; /* (--)
    Options set internally allowing apply() to branch appropriately to set the
    T_inertial->body transformation matrix.*/

  bool z_axis_points_up; /* (--) Used with VelPos and SolarPosPos to orient z */

  bool increment_rot_rate_init_with_pfix; /* (--)
         When attitude rate is specified wrt a pfix frame, the rate of the
         pfix frame needs adding.  Must be transformed to body frame first.*/


  // Derived states
  double pfix_position[3]; /* (m)
     Position vector expressed relative to the pfix frame. */
  double pfix_velocity[3]; /* (m/s)
     Velocity expressed in pfix frame.  May or may not be wrt pfix frame;
     see "velocity_is_relative_to_inertial" for associated specification
     of which frame the velocity is with respect to. */

  double omega_cross_r[3]; /* (m/s)
         difference between pfix-expressed and inertial-expressed velocity.
         May be expressed in either inertial or pfix depending on context.*/

  double T_pfix_reference[3][3]; /* (--)
         transform from pfix to some user-defined reference frame.*/
  double T_inrtl_reference[3][3]; /* (--)
         transform from inertial to some user-defined reference frame.*/
  double T_reference_body[3][3]; /* (--)
         transform from some user-defined reference frame to vehicle body
         frame.*/

  double E_pfix_reference[3]; /* (rad) Euler angles from pfix to reference frame */
  double E_reference_body[3]; /* (rad) Euler angles from reference frame to body */

  jeod::Orientation::EulerSequence ref_body_sequence; /* (--)
         The RPY/PRY etc sequence. */

  jeod::AltLatLongState pfix_ref_point; /* (--)
       the alt/lat/long of the reference point. */
  jeod::NorthEastDown   pfix_ref_point_state; /* (--)
       The NED state of the reference point */


  // Other useful values
  double random_value; /* (--)
       Simple random number. Class element for debugging */
  double random_unit_vector[3]; /* (--) a random unit vector */

  bool force_match_trans;   /* (--)
       The 0th and 1st derivative (pos/vel) are grouped together.
       Some initialization options require that companion state-set
       be initialized using the same option.*/
  bool force_match_rot;   /* (--)
       The 0th and 1st derivative (att/att-rate) are grouped together.
       Some initialization options require that companion state-set
       be initialized using the same option.*/

 public:
  StateInitialize();
  virtual ~StateInitialize(){};

  virtual void initialize(  jeod::DynManager & dyn_manager );
  virtual void apply(  jeod::DynManager & dyn_manager );
  void overwrite_attitude_from_free_stream( double rel_wind_inrtl[3]);

 protected:
  void select_position_initializer();
  void select_velocity_initializer();
  void select_attitude_initializer();
  void select_att_rate_initializer();

  void generate_pfix_ref_point_state();
  void generate_pfix_position();
  void generate_pfix_velocity();
  void generate_pfix_velocity_from_veh_pos();
  void generate_rot_init_transform();
  void generate_inertial_velocity();
  void generate_random(int seed);
  void verify_compatibility();

  void generate_free_stream_velocity( double rel_wind_inrtl[3]);
  void remove_from_trans_init(  jeod::DynBodyInitTransState::StateItems item);
  void remove_from_rot_init(  jeod::DynBodyInitRotState::StateItems item);

  void generate_orbital_init_values(jeod::DynManager & dyn_manager);
  void generate_trans_init_values();
  void generate_rot_init_values(jeod::DynManager & dyn_manager);
  void apply_internal(jeod::DynManager & dyn_manager);

 private:
  // private and unimplemented makes this non-copyable
  StateInitialize (const StateInitialize &);
  StateInitialize & operator = (const StateInitialize &);
};
#endif
