/*******************************TRICK HEADER******************************
PURPOSE: (Provides a shadow-body, utilizing a trajectory data file for its
          state.)

LIBRARY DEPENDENCY:
   ((../src/shadow_trajectory.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (2017-12) (Antares) (initial)))
**********************************************************************/
#ifndef ANTARES_SHADOW_TRAJECTORY_HH
#define ANTARES_SHADOW_TRAJECTORY_HH

#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"
#include "jeod/models/dynamics/dyn_manager/include/base_dyn_manager.hh"
#include "cml/models/vehicle_management/dummy_vehicle_launcher/include/dummy_vehicle_launcher.hh"
#include "cml/models/dynamics/state_overrides/traj_model/include/secondary_traj_model.hh"


/*****************************************************************************
ShadowTrajectory
Purpose:
  Provides a replica body whose state will be updated based on a
  prescribed trajectory/state file that had previously been associated
  with another body.  The trajectory file may (but is not required to)
  have been associated with the same body that is being replicated.
Notes:
- Registering the body with the Dynamics Manager is required to use the
  Prescribed Trajectory Model as of the 2025 refactor.
- Registering the body with the Dynamics Manager also supports:
  - the expression of the relative state of the intrinsic reference frames
    with respect to other reference frames in the sim.
  - creation of additional reference frames within the body.
  - transition to an integrated state
    - but note that typically forces and torques will not be collected, so
      without significant extra effort, this is restricted to straight-line
      trajectories only.
*****************************************************************************/
class ShadowTrajectory : public SubscriptionBase {
 public:
  jeod::DynBody     body;    /* (--)
    Shadow body, replica of primary body */
  DummyVehicleLauncher  launcher;/* (--)
    Dummy Vehicle Launcher which copies over mass/pos/vel/accel state info
    from a real body to the shadow body  */
  SecondaryTrajModel    traj;    /* (--)
    Prescribed trajectory associated with the shadow body */
  bool use_integrated_state;     /* (--)
    Optional flag, indicates that the shadow-body should use an integrated
    state instead of the prescribed trajectory.
    Default: false. */
 private:
  bool use_trans_dynamics; /* (--)
    Flag to record the body's translational_dynamics flag at model
    initialization. This must be set to false before the body gets initialized
    by the Dynamics Manager, and gets reset to its original value at model
    activation after the body has a legitimate state and mass properties.*/
  bool use_rot_dynamics; /* (--)
    Flag to record the body's rotational_dynamics flag at model
    initialization. This must be set to false before the body gets initialized
    by the Dynamics Manager, and gets reset to its original value at model
    activation after the body has a legitimate state and mass properties.*/

 public:
  ShadowTrajectory( const jeod::DynBody & real_body,
                    const TrajModel         & trajectory_data);
  virtual ~ShadowTrajectory(){};

  void initialize() override;
  void initialize( jeod::BaseDynManager & dyn_manager);
  void initialize_integ();
  void update();

 protected:
  void activate() override;

 private:
  ShadowTrajectory (const ShadowTrajectory&) = delete;
  ShadowTrajectory & operator = (const ShadowTrajectory&) = delete;
};
#endif
