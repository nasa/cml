/*******************************TRICK HEADER******************************
PURPOSE: (Provides a shadow-body, utilizing a trajectory data file for its
          state.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (2017-12) (Antares) (initial))
   ((Gary Turner) (OSR) (2025-05) (ANTARES)
        (Update in response to tighter requirements on SecondaryTrajModel))
  )
**********************************************************************/

#include "../include/shadow_trajectory.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
ShadowTrajectory::ShadowTrajectory(
  const jeod::DynBody & real_body,
  const TrajModel     & trajectory_data)
  :
  body(),
  launcher(body,
           real_body), // use real body for both mass and state
  traj( trajectory_data,
        body),
  use_integrated_state(false),
  use_trans_dynamics(false),
  use_rot_dynamics(false)

{
  subscribe_name = "ShadowTrajectory:";

  /* Have the launcher copy the rot state as well as the trans state (which
     is copied by default), and associate body with the same integ group
     that the real-body is integrated by.*/
  launcher.copy_rot_state = true;
  launcher.set_intended_integ_body_state_body();

  // Secondary Traj Model will post a warning message about not being attached
  // unless instructed not to.
  traj.ignore_secondary_warning = true;

  /* Secondary Traj Model will create new reference frames on the secondary
   * body (i.e. the body being created by this model) that mirror those on
   * the primary body (i.e. real_body) that were used to populate the state
   * with the prescribed-trajectory data. The offset from this body's
   * structure frame to those new frames should be set to be equal to the
   * offset from the primary-body's structure frame to the original target
   * frames. So we need to use the primary-body structure frame to compute the
   * offsets.*/
  traj.set_copy_struc_frame_to_primary();
}

/*****************************************************************************
initialize
Purpose:(initializes the model)
*****************************************************************************/
void
ShadowTrajectory::initialize()
{
  CMLMessage::error( __FILE__,__LINE__,
    "Shadow Trajectory isntances must be initialized with a "
    "Dynamics-manager: initialize(BaseDynManager&).\n"
    "The argument-free initialize() method is not supported.\n");
}
/****************************************************************************/
void
ShadowTrajectory::initialize(
     jeod::BaseDynManager & dyn_manager)
{
  traj.initialize();
  use_trans_dynamics = body.translational_dynamics;
  use_rot_dynamics = body.rotational_dynamics;
  body.translational_dynamics = false;
  body.rotational_dynamics = false;
  body.initialize_model(dyn_manager);
  SubscriptionBase::initialize();
}
/****************************************************************************/
void
ShadowTrajectory::initialize_integ()
{
  launcher.initialize_integ_group_actions();
}

/*****************************************************************************
activate
Purpose:
  activates the shadow trajectory
Limitation:
  This method should never be run while inside an integration cycle.
Notes:
  Requires initialization, which means the body has been registered
  with the Dynamics Manager. If this model ever gets modified, DO NOT PROCEED
  past this point without a guarantee thaat the body has been
  registered with the Dyn-manager.
*****************************************************************************/
void
ShadowTrajectory::activate()
{
  /* "create" the new body with mass and dynamics state matching that of
     the real body.  Note - the real body is known by the launcher model.*/
  launcher.launch();

  /* This next step may seema  bit redundant, launcher.launch() just set the
   * states after all. But launcher.launch() may be used to set the state of
   * a DynBody not managed by the DynManager, so it cannot use
   * DynBody::set_* and just copies data in. That leaves DynBody without
   * knowledge of where the data came from, and without knowledge of attitude
   * and rate data, the position and velocity cannot be set properly.
   * So now use the set_* methods basically to tell the DynBody that it has
   * these data values for real.*/
  body.set_position(
         body.composite_body.state.trans.position,
         body.composite_body);
  body.set_velocity(
         body.composite_body.state.trans.velocity,
         body.composite_body);
  body.set_attitude_left_quaternion(
         body.composite_body.state.rot.Q_parent_this,
         body.composite_body);
  body.set_attitude_rate(
         body.composite_body.state.rot.ang_vel_this,
         body.composite_body);

  /* Default operation is to use the prescribed trajectory to set the state
   * of the new body from here on out.  However, there are times when it may
   * be desirable to integrate the state of the new body instead. Note,
   * however, that this will likely be limited to ballistic trajectories
   * because it is unlikely that forces will be collected for this body.
   * In thiss case, we simply turn on the dynamics flags but do not activate
   * the model.*/
  if (use_integrated_state) {
    body.translational_dynamics = true;
    body.rotational_dynamics = true;
    return;
  }

  /* turn off state-integration and subscribe to the prescribed trajectory
   * model.*/
  body.translational_dynamics = use_trans_dynamics;
  body.rotational_dynamics = use_rot_dynamics;
  traj.subscribe();
  traj.update();

  // push up the activation command to the parent class.
  SubscriptionBase::activate();
}


/*****************************************************************************
update
Purpose:(updates the vehicle state from the trajectory file)
*****************************************************************************/
void
ShadowTrajectory::update()
{
  /* Even though traj.update() has a similar active gate, we keep this one
     for extra safety.  There is no this->deactivate() which deactivates traj
     so a user would have to know to deactivate the traj member as well.*/
  if (!active) {
    return;
  }
  traj.update();
}
