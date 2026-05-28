dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_SinglePlanet
dynamics.dyn_manager_init.central_point_name = "Earth"

vehicle.body.integ_frame_name = "Earth.inertial"
vehicle.body.set_name("test_vehicle")
vehicle.body.translational_dynamics = True
vehicle.body.rotational_dynamics = True

# Mass
vehicle.mass_init.set_subject_body( vehicle.body )
vehicle.mass_init.properties.mass = 1.0
vehicle.mass_init.properties.position  = [0,0,0]
vehicle.mass_init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
vehicle.mass_init.properties.inertia = [[1,0,0],[0,1,0],[0,0,1]]
vehicle.mass_init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
vehicle.mass_init.properties.pt_orientation.trans = [[1,0,0],[0,1,0],[0,0,1]]


dynamics.dyn_manager.add_body_action( vehicle.mass_init)

# Gravity
vehicle.grav_controls_earth.source_name = "Earth"
vehicle.grav_controls_earth.active = True
vehicle.grav_controls_earth.spherical = True
vehicle.grav_controls_earth.gradient = False
vehicle.body.add_control( vehicle.grav_controls_earth )

