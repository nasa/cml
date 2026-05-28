/*******************************************************************************
PURPOSE: (Allows the specification of a known state at a time other than the
          desired simulation-initialization-time.  This state can be propagated
          to the desired simulation-start-time and then used to initialize the
          simulation.)

ASSUMPTIONS AND LIMITATIONS:
   (( Initial propagation assumes that the primary gravity-body rotates on
      its z-axis, and that 3rd-bodies are static for the duration of the
      "make-up" integration.  Once the sim starts in full, the integration
      will be conducted using normal procedures.
     )
   ( Where possible, the simulation should just be run normally for the
     desired propagation time; any sim may be run in reverse if so desired by
     setting the time-model's scale-factor to a negative value.
     This model is provided for more simulations that are complicated with the
     presence of models that have one or more of the following problems:
     - implicit assumptions about start-time being zero,
     - implicit assumptions about the direction of time,
     - are cued off the simulation-counter rather than the dynamic-time
     - an initialization method that depends on initial state and no method
       to re-initialize when the initial state is truly known
     - an initialization method that relies on some pre-defined configuration,
       defined at the intended simulation start time but which is inconsistent
       with the configuration at the known-state's-time; this is only an issue
       in cases where the wrong configuration will interfere with the
       integration from the known-state's-time to the
       intended-simulation-start-time
     - any sort of sensor-effector feedback mechanism that will be invalidated
       by the integration; this is usually only applicable where it is
       necessary to integrate backward in time.
   )

PROGRAMMERS:
   (((Gary Turner) (OSR) (October 2016) (New))
    ((Bingquan Wang) (OSR) (April 2017)
       (Disabled the compilation warning of float-point number equality
        comparison))
   )
 ******************************************************************************/

#include <cmath>   // abs
#include <cstddef> // NULL

#include "../include/state_initialize_with_propagation.hh"

/*****************************************************************************
constructor
*****************************************************************************/
StateInitializeWithPropagation::StateInitializeWithPropagation(
    jeod::GravityManager           &  gravity_manager_in)
  :
  StateInitialize(),
  gravity_manager( gravity_manager_in),
  body(NULL),
  T_initial_to_current{{1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{0.0, 0.0, 1.0}},
  T_inrtl_to_initial_pfix{{1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{0.0, 0.0, 1.0}},
  omega(0.0),
  propagation_time(0.0),
  time_step(1.0),
  apply_dispersions_before_propagation(true)
{}

/*****************************************************************************
initialize
Purpose:(initializes the body-action in preparation for application)
*****************************************************************************/
void
StateInitializeWithPropagation::initialize( jeod::DynManager & dyn_manager)
{
  StateInitialize::initialize( dyn_manager); // sets the planet pointer to
                                             // non-null or fails.
  body = get_subject_dyn_body();
  if (body == NULL) {
    CMLMessage::fail(
    __FILE__,__LINE__,"Invalid subject specification\n",
    "The subject of this body action, is NULL\n");
  }
  // initialized flag set in StateInitialize::initialize().  There is no other
  // way to set the initialized flag than by going through this method.
  // So if the initialized flag is set and the code is still running, the
  // planet and body pointers have been checked for non-NULL.
  // Both pointers are protected, so it is sufficient to check the initialized
  // flag as a proxy for (planet != NULL && body != NULL).
}

/*****************************************************************************
apply
Purpose:(Applies the state_initialize action)
*****************************************************************************/
void
StateInitializeWithPropagation::apply( jeod::DynManager & dyn_manager)
{
  if (!initialized) {
    CMLMessage::error(
      __FILE__,__LINE__,"Premature application of Body Action.\n"
      "This body-action ('", action_identifier, "') must be initialized before it can be applied.\n"
      "Application failed.\n");
    return;
  }

  if (use_orbital_init) {
    generate_orbital_init_values(dyn_manager);
    //includes call to DynBodyInitOrbit::apply
  }
  if (use_trans_init) {
    generate_trans_init_values();

    if (!MathUtils::is_near_equal(propagation_time, 0.0)) {
      propagate_state();
    }
    else {
      CMLMessage::warn(
      __FILE__,__LINE__,
      "Redundant use of class StateInitializeWithPropagation\n",
      "There is nothing wrong per se, but the use of the\n"
      "StateInitializeWithPropagation class generally implies that the\n"
      "translational state needs propagating for initialization.\n"
      "However, the propagation-time has been set to 0.0.\n"
      "Check your configuration.\n");
    }
  }
  else {
    CMLMessage::warn(
    __FILE__,__LINE__,
    "Redundant use of class StateInitializeWithPropagation\n",
    "There is nothing wrong per se, but the use of the\n"
    "StateInitializeWithPropagation class generally implies that the\n"
    "translational state needs propagating for initialization.\n"
    "However, use of this class's translational state initialization methods \n"
    "have been disabled.\nCheck your configuration.\n");
  }

  if (use_rot_init) {
    generate_rot_init_values( dyn_manager);
  }

  // apply correlations
  if (correlation.dispersion_distribution !=
                      CorrelatedStateDispersion::NoDispersion  ) {
    // If dispersions are to be applied after propagation, go ahead
    if (!apply_dispersions_before_propagation) {
      correlation.disperse_state( trans_init.position,
                                  trans_init.velocity,
                                  rot_init.orientation.trans);
    }
    // Otherwise, if there are attitude dispersions to be made, apply those
    else if ( correlation.corr_option ==
                        CorrelatedStateDispersion::CORRELATED_PV_ATT) {
      correlation.apply_attitude_dispersions( rot_init.orientation.trans);
    }
    // else, PV has already been dispersed and attitude is not needed.  Done.
  }
  // Apply the actual state values now.
  apply_internal( dyn_manager);
}

/*****************************************************************************
propagate_state
Purpose:(Propagates the specified state through the desired time)
Definitions: (
    (state-time is the time at which the translational state is already known,
        this is the time that corresponds to the given position/velocity as
        specified in the input file for this instance of StateInitialize.)
    (spec-time is the time at which the sim should start, this is the time to
     which the known state will be propagated in this method)
    (NOTE: This implies state-time is "behind" spec-time for forward-propagation
        (propagation_time > 0) and state-time is "ahead" of spec-time for
        reverse-propagation (propagation_time < 0)) )
Assumptions: planetary rotation is purely z-axis
*****************************************************************************/
void
StateInitializeWithPropagation::propagate_state()
{
  // planet orientation has already been aligned with the specified
  // simulation-start-time (spec-time).  However, the known state is not
  // at that time, so we need to shift the environment to match what it would
  // be at the state's time (state-time), then integrate towards spec-time.

  // Make a copy of the initial (spec-time) inertial->pfix transformation matrix
  // and obtain the angular rate of the planet's pfix frame relative to inertial
  // ASSUMPTION - rotation is purely z-axis
  jeod::Matrix3x3::copy (planet->pfix.state.rot.T_parent_this, T_inrtl_to_initial_pfix);
  omega = planet->pfix.state.rot.ang_vel_this[2];

  //Rotate planet to state-time for proper planet-relative initialization
  compute_planet_orientation( -propagation_time );

  // Generate the trans-init values now.  This is important for cases in which
  // the specified state is expressed in ways other than inertial.  If it is
  // already expressed in inertial, this step is redundant but harmless.
  StateInitialize::generate_trans_init_values();

  // If dispersions should be applied before propagation, apply them now
  if (apply_dispersions_before_propagation &&
     (correlation.dispersion_distribution !=
                     CorrelatedStateDispersion::NoDispersion)) {
    correlation.disperse_state( trans_init.position,
                                trans_init.velocity);
  }

  // propagation only supports RK4 integration.
  rk4_integration();

  // revert the planet-orientation to its original value
  jeod::Matrix3x3::copy ( T_inrtl_to_initial_pfix,
                    planet->pfix.state.rot.T_parent_this);
}

/*****************************************************************************
rk4_integration
Purpose:(Use Runge-Kutta 4 integration to propagate the PV state)
*****************************************************************************/
void
StateInitializeWithPropagation::rk4_integration()
{
  // Adjust the specified step size so that it hits the target perfectly
  unsigned int num_steps(std::abs( propagation_time / time_step) + 0.5);
  time_step = propagation_time / num_steps;


  double integ_pos[3];
  double integ_vel[3];
  double scratch_pos[3];
  double interim_pos_dot[4][3];  // Holds constants for each RK step
  double interim_vel_dot[4][3];  // Holds constants for each RK step

  jeod::Vector3::copy( trans_init.position, integ_pos);
  jeod::Vector3::copy( trans_init.velocity, integ_vel);

  for (unsigned int ii_step = 0; ii_step < num_steps; ii_step++) {
    double time_from_env_config = ii_step * time_step - propagation_time;
    compute_planet_orientation( time_from_env_config);

    // initialize the interim velocities
    for (unsigned int jj = 0; jj< 4; jj++) {
      jeod::Vector3::copy( integ_vel, interim_pos_dot[jj]);
    }

    // Compute k1 (interim_pos_dot[0], interim_vel_dot[0])
    gravity_manager.gravitation( integ_pos, body->grav_interaction);
    jeod::Vector3::copy( body->grav_interaction.grav_accel, interim_vel_dot[0]);

    // Compute k2
    compute_planet_orientation( time_from_env_config + time_step / 2.0);
    //                        input              input h/2        output k2
    jeod::Vector3::scale_incr( interim_vel_dot[0], (time_step/2.0), interim_pos_dot[1]);

    jeod::Vector3::copy( integ_pos, scratch_pos);
    //                        input              input h/2        output
    jeod::Vector3::scale_incr( interim_pos_dot[0], (time_step/2.0), scratch_pos);
    gravity_manager.gravitation( scratch_pos, body->grav_interaction);
    //                        input                      output k2
    jeod::Vector3::copy( body->grav_interaction.grav_accel, interim_vel_dot[1]);


    // Compute k3
    //                     input k2            input h/2           output k3
    jeod::Vector3::scale_incr( interim_vel_dot[1], (time_step/2.0), interim_pos_dot[2]);

    jeod::Vector3::copy( integ_pos, scratch_pos);
    //                      input k2           input h/2        output
    jeod::Vector3::scale_incr( interim_pos_dot[1], (time_step/2.0), scratch_pos);
    gravity_manager.gravitation( scratch_pos, body->grav_interaction);
    //                        input                      output k3
    jeod::Vector3::copy( body->grav_interaction.grav_accel, interim_vel_dot[2]);

    // Compute k4:
    compute_planet_orientation( time_from_env_config + time_step);

    //                     input k3            input h          output k4
    jeod::Vector3::scale_incr( interim_vel_dot[2], time_step, interim_pos_dot[3]);
    jeod::Vector3::copy( integ_pos, scratch_pos);
    //                     input k3            input h          output
    jeod::Vector3::scale_incr( interim_pos_dot[2], time_step, scratch_pos);
    gravity_manager.gravitation( scratch_pos, body->grav_interaction);
    //                        input                      output k4
    jeod::Vector3::copy( body->grav_interaction.grav_accel, interim_vel_dot[3]);

    double sum_pos_dot[3];
    double sum_vel_dot[3];
    jeod::Vector3::initialize(sum_pos_dot);
    jeod::Vector3::initialize(sum_vel_dot);
    //  y_n+1 =   y_n + h/6 ( k1  + 2k2 + 2k3 + k4)
    for (unsigned int jj = 0; jj<3; jj++) {
      sum_pos_dot[jj] =   interim_pos_dot[0][jj] + 2*interim_pos_dot[1][jj] +
                        2*interim_pos_dot[2][jj] +   interim_pos_dot[3][jj];
      sum_vel_dot[jj] =   interim_vel_dot[0][jj] + 2*interim_vel_dot[1][jj] +
                        2*interim_vel_dot[2][jj] +   interim_vel_dot[3][jj];
    }
    jeod::Vector3::scale_incr( sum_pos_dot, (time_step / 6.0), integ_pos);
    jeod::Vector3::scale_incr( sum_vel_dot, (time_step / 6.0), integ_vel);

  }
  jeod::Vector3::copy( integ_pos, trans_init.position);
  jeod::Vector3::copy( integ_vel, trans_init.velocity);
}

/*****************************************************************************
compute_planet_orientation
Purpose:(A really quick and dirty way to rotate the planet so that the
         gravity is oriented correctly)
*****************************************************************************/
void
StateInitializeWithPropagation::compute_planet_orientation(
       double time )
{
  double cos_wt = std::cos( omega * time);
  double sin_wt = std::sin( omega * time);

  T_initial_to_current[0][0] = cos_wt;
  T_initial_to_current[0][1] = sin_wt;
  T_initial_to_current[1][0] = -sin_wt;
  T_initial_to_current[1][1] = cos_wt;

  jeod::Matrix3x3::product( T_initial_to_current,
                      T_inrtl_to_initial_pfix,
                      planet->pfix.state.rot.T_parent_this);
}
