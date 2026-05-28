dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_SinglePlanet
dynamics.dyn_manager_init.central_point_name = "Earth"

scalar_integ.body.integ_frame_name = "Earth.inertial"
scalar_integ.body.set_name("test_vehicle")
scalar_integ.body.translational_dynamics = True
scalar_integ.body.rotational_dynamics = True

# Mass
scalar_integ.mass_init.set_subject_body( scalar_integ.body )
scalar_integ.mass_init.properties.mass = 1.0
scalar_integ.mass_init.properties.position  = [0,0,0]
scalar_integ.mass_init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
scalar_integ.mass_init.properties.inertia = [[1,0,0],[0,1,0],[0,0,1]]
scalar_integ.mass_init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
scalar_integ.mass_init.properties.pt_orientation.trans = [[1,0,0],[0,1,0],[0,0,1]]


dynamics.dyn_manager.add_body_action( scalar_integ.mass_init)

# Gravity
scalar_integ.grav_controls_earth.source_name = "Earth"
scalar_integ.grav_controls_earth.active = True
scalar_integ.grav_controls_earth.spherical = True
scalar_integ.grav_controls_earth.gradient = False
scalar_integ.body.add_control( scalar_integ.grav_controls_earth )

#State
scalar_integ.state_initialize.set_subject_body( scalar_integ.body )
scalar_integ.state_initialize.body_frame_id = "composite_body"
scalar_integ.state_initialize.planet_name = "Earth"

scalar_integ.state_initialize.position_input_data_type = trick.StateInitialize.Inertial
scalar_integ.state_initialize.velocity_input_data_type = trick.StateInitialize.Inertial
scalar_integ.state_initialize.attitude_input_data_type = trick.StateInitialize.EulerInertial_YPR
scalar_integ.state_initialize.att_rate_input_data_type = trick.StateInitialize.BodyInertialRate
scalar_integ.state_initialize.yaw = 0.0
scalar_integ.state_initialize.pitch = 0.0
scalar_integ.state_initialize.roll = 0.0
scalar_integ.state_initialize.roll_rate_body = 0.0
scalar_integ.state_initialize.pitch_rate_body = 0.0
scalar_integ.state_initialize.yaw_rate_body = 0.0
scalar_integ.state_initialize.position = [6800000.0, 0.0, 0.0]
scalar_integ.state_initialize.velocity = [0.0, 8000.0, 0.0]
scalar_integ.state_initialize.active = True
dynamics.dyn_manager.add_body_action( scalar_integ.state_initialize)
