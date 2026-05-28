dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_Ephemerides

env.de4xx.set_model_number(421)

jeod_time.time_manager_init.initializer = "UTC"
jeod_time.time_manager_init.sim_start_format = trick.TimeEnum.calendar
jeod_time.time_utc.calendar_year = 2017
jeod_time.time_utc.calendar_month = 1
jeod_time.time_utc.calendar_day = 1
jeod_time.time_utc.calendar_hour = 0
jeod_time.time_utc.calendar_minute = 0
jeod_time.time_utc.calendar_second = 0

vehicle.body.integ_frame_name = "Earth.inertial"
vehicle.body.set_name("test_vehicle")
vehicle.body.translational_dynamics = False
vehicle.body.rotational_dynamics = False

# Mass
vehicle.mass_init.set_subject_body( vehicle.body )
vehicle.mass_init.properties.mass = 1.0
vehicle.mass_init.properties.position  = [0,0,0]
vehicle.mass_init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
vehicle.mass_init.properties.inertia = [[1,0,0],[0,1,0],[0,0,1]]
vehicle.mass_init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
vehicle.mass_init.properties.pt_orientation.trans = [[1,0,0],[0,1,0],[0,0,1]]


dynamics.dyn_manager.add_body_action( vehicle.mass_init)

#State
vehicle.state_initialize.set_subject_body( vehicle.body )
vehicle.state_initialize.body_frame_id = "composite_body"
vehicle.state_initialize.planet_name = "Earth"

vehicle.state_initialize.position_input_data_type = trick.StateInitialize.Inertial
vehicle.state_initialize.velocity_input_data_type = trick.StateInitialize.Inertial
vehicle.state_initialize.attitude_input_data_type = trick.StateInitialize.EulerInertial_YPR
vehicle.state_initialize.att_rate_input_data_type = trick.StateInitialize.BodyInertialRate
vehicle.state_initialize.yaw = 0.0
vehicle.state_initialize.pitch = 0.0
vehicle.state_initialize.roll = 0.0
vehicle.state_initialize.roll_rate_body = 0.0
vehicle.state_initialize.pitch_rate_body = 0.0
vehicle.state_initialize.yaw_rate_body = 0.0
vehicle.state_initialize.position = [6800000.0, 0.0, 0.0]
vehicle.state_initialize.velocity = [0.0, 8000.0, 0.0]
vehicle.state_initialize.active = True
dynamics.dyn_manager.add_body_action( vehicle.state_initialize)


vehicle.rel_state.subject_frame_name = "test_vehicle.composite_body"
vehicle.rel_state.target_frame_name = "EMBary.em_rot"
vehicle.rel_state.direction_sense = trick.RelativeDerivedState.ComputeSubjectStateinTarget
