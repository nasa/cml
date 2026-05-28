dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_SinglePlanet
dynamics.dyn_manager_init.central_point_name = "Earth"

vehicle.body.integ_frame_name = "Earth.inertial"
vehicle.body.set_name("test_vehicle")
vehicle.body.translational_dynamics = True
vehicle.body.rotational_dynamics = True

vehicle.group.subscribe()

#State
vehicle.state_initialize.set_subject_body( vehicle.body )
vehicle.state_initialize.body_frame_id = "composite_body"
vehicle.state_initialize.planet_name = "Earth"

vehicle.state_initialize.position_input_data_type = trick.StateInitialize.Inertial
vehicle.state_initialize.velocity_input_data_type = trick.StateInitialize.Inertial
vehicle.state_initialize.attitude_input_data_type = trick.StateInitialize.ICOPT_EUL_YPR_INERT_BODY
vehicle.state_initialize.att_rate_input_data_type = trick.StateInitialize.BodyInertialRate
vehicle.state_initialize.yaw = 0.0
vehicle.state_initialize.pitch = 0.0
vehicle.state_initialize.roll = 0.0
vehicle.state_initialize.roll_rate_body = 0.0
vehicle.state_initialize.pitch_rate_body = 0.0
vehicle.state_initialize.yaw_rate_body = 0.0
vehicle.state_initialize.position = [100.0, 0.0, 0.0]
vehicle.state_initialize.velocity = [0.0, 0.0, 0.0]
vehicle.state_initialize.active = True
dynamics.dyn_manager.add_body_action( vehicle.state_initialize)
