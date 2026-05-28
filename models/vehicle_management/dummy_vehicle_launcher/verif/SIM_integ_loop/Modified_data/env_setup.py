dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_SinglePlanet
dynamics.dyn_manager_init.central_point_name = "Earth"

# Dyn body
vehicle.dyn_body.set_name("main_body")
vehicle.dyn_body.integ_frame_name = "Earth.inertial"
vehicle.dyn_body.translational_dynamics = True
vehicle.dyn_body.rotational_dynamics = True

vehicle2.dyn_body.set_name("launch_body")
vehicle2.dyn_body.integ_frame_name = "Earth.inertial"
vehicle2.dyn_body.translational_dynamics = False
vehicle2.dyn_body.rotational_dynamics = False

# Mass
vehicle.mass_init.set_subject_body(vehicle.dyn_body)
vehicle.mass_init.properties.mass = 100.0
vehicle.mass_init.properties.position  = [0,0,0]
vehicle.mass_init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
vehicle.mass_init.properties.inertia = [[1,0,0],[0,1,0],[0,0,1]]
vehicle.mass_init.properties.pt_orientation.data_source = trick.Orientation.InputEigenRotation
vehicle.mass_init.properties.pt_orientation.eigen_angle = 0.0
vehicle.mass_init.properties.pt_orientation.eigen_axis = [0.0, 1.0, 0.0]

dynamics.dyn_manager.add_body_action( vehicle.mass_init)

#State
vehicle.trans_init.set_subject_body(vehicle.dyn_body)
vehicle.trans_init.reference_ref_frame_name = "Earth.inertial"
vehicle.trans_init.body_frame_id           = "composite_body"
vehicle.trans_init.position  = trick.sim_services.attach_units("m" ,   [7000000.0 , 0.0 , 0.0]) 
vehicle.trans_init.velocity  = trick.sim_services.attach_units("m/s" , [0.0 , 4000.0 , 0.0]) 

vehicle.rot_init.set_subject_body(vehicle.dyn_body)
vehicle.rot_init.reference_ref_frame_name    = "Earth.inertial"
vehicle.rot_init.body_frame_id              = "composite_body"
vehicle.rot_init.orientation.data_source     = trick.Orientation.InputEigenRotation
vehicle.rot_init.orientation.eigen_angle     = trick.sim_services.attach_units("degree",180.0)
vehicle.rot_init.orientation.eigen_axis     = [0.0,0.0,1.0] 
vehicle.rot_init.ang_velocity               = trick.sim_services.attach_units("degree/s" , [0.0, -45.0, 0.0])

dynamics.dyn_manager.add_body_action( vehicle.trans_init)
dynamics.dyn_manager.add_body_action( vehicle.rot_init)

# Gravity
vehicle.grav_control.source_name = "Earth"
vehicle.grav_control.active = True
vehicle.grav_control.spherical = True
vehicle.dyn_body.grav_interaction.add_control(vehicle.grav_control)

vehicle2.grav_control.source_name = "Earth"
vehicle2.grav_control.active = True
vehicle2.grav_control.spherical = True
vehicle2.dyn_body.grav_interaction.add_control(vehicle2.grav_control)

# Mass body
dummy_veh_launcher_verif.mass_body.composite_properties.mass = 20.0;
dummy_veh_launcher_verif.mass_body.composite_properties.position[0] = 7000000.0;
dummy_veh_launcher_verif.mass_body.composite_properties.position[1] = 1.0;
dummy_veh_launcher_verif.mass_body.composite_properties.position[2] = 2.0;
dummy_veh_launcher_verif.mass_body.composite_properties.inertia[0][0] = 1.0;
dummy_veh_launcher_verif.mass_body.composite_properties.inertia[0][1] = 0.0;
dummy_veh_launcher_verif.mass_body.composite_properties.inertia[0][2] = 0.0;
dummy_veh_launcher_verif.mass_body.composite_properties.inertia[1][0] = 0.0;
dummy_veh_launcher_verif.mass_body.composite_properties.inertia[1][1] = 1.0;
dummy_veh_launcher_verif.mass_body.composite_properties.inertia[1][2] = 0.0;
dummy_veh_launcher_verif.mass_body.composite_properties.inertia[2][0] = 0.0;
dummy_veh_launcher_verif.mass_body.composite_properties.inertia[2][1] = 0.0;
dummy_veh_launcher_verif.mass_body.composite_properties.inertia[2][2] = 1.0;
