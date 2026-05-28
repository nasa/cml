dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4

test.body.integ_frame_name = "Earth.inertial"
test.body.set_name("test_veh")
test.body.translational_dynamics = True
test.body.rotational_dynamics = True

jeod_time.time_manager_init.initializer = "TAI"
jeod_time.time_manager_init.sim_start_format = trick.TimeEnum.seconds_since_epoch
jeod_time.time_tai.initializing_value = 0
  
# Mass
test.mass_init.set_subject_body( test.body )
test.mass_init.properties.mass = 1.0
test.mass_init.properties.position  = [0,0,0]
test.mass_init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
test.mass_init.properties.inertia = [[1,0,0],[0,1,0],[0,0,1]]
test.mass_init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
test.mass_init.properties.pt_orientation.trans = [[1,0,0],[0,1,0],[0,0,1]]


dynamics.dyn_manager.add_body_action( test.mass_init)

# Gravity
test.grav_controls_earth.source_name = "Earth"
test.grav_controls_earth.active = True
test.grav_controls_earth.spherical = False
test.grav_controls_earth.degree = 4
test.grav_controls_earth.order = 4
test.grav_controls_earth.gradient = False
test.body.add_control( test.grav_controls_earth )

test.grav_controls_sun.source_name = "Sun"
test.grav_controls_sun.active = True
test.grav_controls_sun.spherical = True
test.body.add_control( test.grav_controls_sun )

test.grav_controls_moon.source_name = "Moon"
test.grav_controls_moon.active = True
test.grav_controls_moon.spherical = True
test.body.add_control( test.grav_controls_moon )

#State
test.state_init.set_subject_body( test.body )
test.state_init.body_frame_id = "composite_body"
test.state_init.planet_name = "Earth"

test.state_init.position_input_data_type = trick.StateInitialize.Inertial
test.state_init.velocity_input_data_type = trick.StateInitialize.Inertial
test.state_init.attitude_input_data_type = trick.StateInitialize.ICOPT_EUL_YPR_INERT_BODY
test.state_init.att_rate_input_data_type = trick.StateInitialize.BodyInertialRate
test.state_init.yaw = 0.0
test.state_init.pitch = 0.0
test.state_init.roll = 0.0
test.state_init.roll_rate_body = 0.0
test.state_init.pitch_rate_body = 0.0
test.state_init.yaw_rate_body = 0.0
test.state_init.position = [6800000.0, 0.0, 0.0]
test.state_init.velocity = [0.0, 8000.0, 0.0]
test.state_init.active = True
dynamics.dyn_manager.add_body_action( test.state_init)

