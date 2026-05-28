dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_EmptySpace
dynamics.dyn_manager_init.central_point_name = "Space"

vehicle.body.integ_frame_name = "Space.inertial"
vehicle.body.set_name("test_vehicle")
vehicle.body.translational_dynamics = True
vehicle.body.rotational_dynamics = True

# Vehicle Mass
vehicle.mass_init.set_subject_body( vehicle.body )
vehicle.mass_init.properties.mass = 1.0
vehicle.mass_init.properties.position  = [0,0,0]
vehicle.mass_init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
vehicle.mass_init.properties.inertia = [[1,0,0],[0,1,0],[0,0,1]]
vehicle.mass_init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
vehicle.mass_init.properties.pt_orientation.trans = [[1,0,0],[0,1,0],[0,0,1]]

dynamics.dyn_manager.add_body_action( vehicle.mass_init)

# Tank mass
vehicle.tank_init.set_subject_body( vehicle.tank )
vehicle.tank_init.properties.mass = 10.0
vehicle.tank_init.properties.position  = [0,0,0]
vehicle.tank_init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
vehicle.tank_init.properties.inertia = [[10,0,0],[0,10,0],[0,0,10]]
vehicle.tank_init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
vehicle.tank_init.properties.pt_orientation.trans = [[1,0,0],[0,1,0],[0,0,1]]

dynamics.dyn_manager.add_body_action( vehicle.tank_init)

vehicle.tank_group.subscribe()
vehicle.tank_group.add_mass_to_group( vehicle.tank)
vehicle.mass_consumption = .1

# Attach tank to body
vehicle.tank_attach.set_parent_body( vehicle.body )
vehicle.tank_attach.set_subject_body( vehicle.tank )
vehicle.tank_attach.offset_pstr_cstr_pstr = [0.0,0.0,0.0]
vehicle.tank_attach.pstr_cstr.data_source = trick.Orientation.InputMatrix;
vehicle.tank_attach.pstr_cstr.trans  =  [[1,0,0],[0,1,0],[0,0,1]]
vehicle.tank_attach.active = True

dynamics.dyn_manager.add_body_action( vehicle.tank_attach )


#State
vehicle.trans_init.set_subject_body( vehicle.body )
vehicle.trans_init.body_frame_id = "composite_body"
vehicle.trans_init.reference_ref_frame_name = "Space.inertial"
vehicle.trans_init.position = [0.0, 0.0, 0.0]
vehicle.trans_init.velocity = [0.0, 0.0, 0.0]
dynamics.dyn_manager.add_body_action( vehicle.trans_init )

vehicle.rot_init.set_subject_body( vehicle.body )
vehicle.rot_init.body_frame_id = "composite_body"
vehicle.rot_init.reference_ref_frame_name = "Space.inertial"
vehicle.rot_init.orientation.data_source  = trick.Orientation.InputEigenRotation
vehicle.rot_init.orientation.eigen_angle  = trick.sim_services.attach_units("degree",0.0)
vehicle.rot_init.orientation.eigen_axis   = [0.0,0.0,1.0]
vehicle.rot_init.ang_velocity             = [0.0, 0.0, 1.0]
dynamics.dyn_manager.add_body_action( vehicle.rot_init )
