dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
env.de4xx.set_model_number(421)

# Can add an earth-rotation to the simulation with something like this:
#earth.planet.pfix.state.rot.ang_vel_this[2] = 0.5
# but leaving it out for now to simplify data comparisons.

vehicle.body.integ_frame_name = "Earth.inertial"
vehicle.body.set_name("test_vehicle")

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

# State
vehicle.state_initialize.set_subject_body( vehicle.body )
vehicle.state_initialize.body_frame_id = "composite_body"
vehicle.state_initialize.planet_name = "Earth"

# load the nominal baseline;
# specific parts of this get overridden by the specific test cases
exec(open("Modified_data/nominal_state_init.py").read())
vehicle.state_initialize.active = True
dynamics.dyn_manager.add_body_action( vehicle.state_initialize)
