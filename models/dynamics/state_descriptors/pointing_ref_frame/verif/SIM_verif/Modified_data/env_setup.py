# Creates and configures the arbitrary frames A and B, and the pointing
# reference frame that points from A to B.
# Set up ephemeris mode as EmptySpace
dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_EmptySpace
dynamics.dyn_manager_init.central_point_name = "Space"
space = dynamics.dyn_manager.find_ref_frame("Space.inertial")

# Set-up for arbitrary reference frames A and B
frame.frame_a.set_name("frame_a")
frame.frame_b.set_name("frame_b")
dynamics.dyn_manager.add_ref_frame(frame.frame_a)
dynamics.dyn_manager.add_ref_frame(frame.frame_b)

# Set state for vehicle
vehicle.body.set_name("test_vehicle")
vehicle.body.translational_dynamics = False
vehicle.body.rotational_dynamics = False

vehicle.trans_init.set_subject_body( vehicle.body )
vehicle.trans_init.reference_ref_frame_name = "Space.inertial"
vehicle.trans_init.body_frame_id            = "composite_body"
vehicle.body.integ_frame_name = "Space.inertial"
dynamics.dyn_manager.add_body_action( vehicle.trans_init)

# Set-up pointing reference frame
frame.pointing_ref_frame.pointing_frame.set_name("PointingFrame")
if frame.set_frames == 1:
  frame.pointing_ref_frame.set_originating_frame(frame.frame_a);
  frame.pointing_ref_frame.set_target_frame(frame.frame_b);
#frame.pointing_ref_frame.set_active_status(True)
frame.pointing_ref_frame.subscribe()
dynamics.dyn_manager.add_ref_frame(frame.pointing_ref_frame.pointing_frame)

vehicle.rel_state.subject_frame_name = "test_vehicle.composite_body"
vehicle.rel_state.target_frame_name = "PointingFrame"
vehicle.rel_state.direction_sense = trick.RelativeDerivedState.ComputeSubjectStateinTarget
