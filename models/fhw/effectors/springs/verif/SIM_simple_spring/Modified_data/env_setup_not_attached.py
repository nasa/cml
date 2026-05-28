dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_EmptySpace
dynamics.dyn_manager_init.central_point_name = "Space"

#***************
#* Parent Body *
#***************
veh_reaction.body.integ_frame_name = "Space.inertial"
veh_reaction.body.set_name("parent_vehicle")
veh_reaction.body.translational_dynamics = True
veh_reaction.body.rotational_dynamics = True

# Mass
veh_reaction.mass_init.set_subject_body(veh_reaction.body)
veh_reaction.mass_init.properties.mass         = 1.0
veh_reaction.mass_init.properties.position     = [-1.0,0,0]
veh_reaction.mass_init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
veh_reaction.mass_init.properties.inertia = [[1,0,0],[0,1,0],[0,0,1]]
veh_reaction.mass_init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
veh_reaction.mass_init.properties.pt_orientation.trans = [[1,0,0],[0,1,0],[0,0,1]]

veh_reaction.mass_init.allocate_points(1)
veh_reaction.mass_init.get_mass_point(0).set_name("parent_to_child")
veh_reaction.mass_init.get_mass_point(0).position = [0.0, 0.0, 0.0]
veh_reaction.mass_init.get_mass_point(0).pt_orientation.data_source=trick.Orientation.InputMatrix;
veh_reaction.mass_init.get_mass_point(0).pt_orientation.trans = [[1,0,0],[0,1,0],[0,0,1]]

dynamics.dyn_manager.add_body_action( veh_reaction.mass_init)

#State
veh_reaction.trans_initialize.set_subject_body(veh_reaction.body)
veh_reaction.trans_initialize.body_frame_id = "structure"
veh_reaction.trans_initialize.reference_ref_frame_name = "Space.inertial"
veh_reaction.trans_initialize.position = [0.0, 0.0, 0.0]
veh_reaction.trans_initialize.velocity = [0.0, 0.0, 0.0]

veh_reaction.rot_initialize.set_subject_body(veh_reaction.body)
veh_reaction.rot_initialize.body_frame_id = "structure"
veh_reaction.rot_initialize.reference_ref_frame_name = "Space.inertial"
veh_reaction.rot_initialize.orientation.data_source = trick.Orientation.InputEulerRotation
veh_reaction.rot_initialize.orientation.euler_sequence = trick.sim_services.Yaw_Pitch_Roll
veh_reaction.rot_initialize.orientation.euler_angles = [0.0, 0.0, 0.0]
veh_reaction.rot_initialize.ang_velocity =  [0.0, 0.0, 0.0]

veh_reaction.trans_initialize.active = True
veh_reaction.rot_initialize.active = True
dynamics.dyn_manager.add_body_action( veh_reaction.trans_initialize)
dynamics.dyn_manager.add_body_action( veh_reaction.rot_initialize)


#**************
#* Child Body *
#**************
veh_action.body.integ_frame_name = "Space.inertial"
veh_action.body.set_name("child_vehicle")
veh_action.body.translational_dynamics = True
veh_action.body.rotational_dynamics = True

# Mass
veh_action.mass_init.set_subject_body(veh_action.body)
veh_action.mass_init.properties.mass         = 1.0
veh_action.mass_init.properties.position     = [1.0,0,0]
veh_action.mass_init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
veh_action.mass_init.properties.inertia      = [[1,0,0],[0,1,0],[0,0,1]]
veh_action.mass_init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
veh_action.mass_init.properties.pt_orientation.trans = [[1,0,0],[0,1,0],[0,0,1]]

veh_action.mass_init.allocate_points(1)
veh_action.mass_init.get_mass_point(0).set_name("child_to_parent")
veh_action.mass_init.get_mass_point(0).position = [0.0, 0.0, 0.0]
veh_action.mass_init.get_mass_point(0).pt_orientation.data_source=trick.Orientation.InputMatrix;
veh_action.mass_init.get_mass_point(0).pt_orientation.trans = [[-1,0,0],[0,1,0],[0,0,-1]]
dynamics.dyn_manager.add_body_action( veh_action.mass_init)

#State
veh_action.trans_initialize.set_subject_body(veh_action.body)
veh_action.trans_initialize.body_frame_id = "structure"
veh_action.trans_initialize.reference_ref_frame_name = "Space.inertial"
veh_action.trans_initialize.position = [0.0, 0.0, 0.0]
veh_action.trans_initialize.velocity = [0.0, 0.0, 0.0]

veh_action.rot_initialize.set_subject_body(veh_action.body)
veh_action.rot_initialize.body_frame_id = "structure"
veh_action.rot_initialize.reference_ref_frame_name = "Space.inertial"
veh_action.rot_initialize.orientation.data_source = trick.Orientation.InputEulerRotation
veh_action.rot_initialize.orientation.euler_sequence = trick.sim_services.Yaw_Pitch_Roll
veh_action.rot_initialize.orientation.euler_angles = [0.0, 0.0, 0.0]
veh_action.rot_initialize.ang_velocity =  [0.0, 0.0, 0.0]

veh_action.trans_initialize.active = True
veh_action.rot_initialize.active = True
dynamics.dyn_manager.add_body_action( veh_action.trans_initialize)
dynamics.dyn_manager.add_body_action( veh_action.rot_initialize)

#################################################################################

# Attach child vehicle to parent
#################################################################################
veh_action.attach.set_subject_body(veh_action.body)
veh_action.attach.set_parent_body(veh_reaction.body)
veh_action.attach.active = False
veh_action.attach.offset_pstr_cstr_pstr = [0.0, 0.0, 0.0]
veh_action.attach.pstr_cstr.data_source = trick.Orientation.InputMatrix
veh_action.attach.pstr_cstr.trans = [[1,0,0],[0,1,0],[0,0,1]]
dynamics.dyn_manager.add_body_action( veh_action.attach)

#################################################################################
veh_action.detach.set_subject_body(veh_action.body)
veh_action.detach.active = False
dynamics.dyn_manager.add_body_action( veh_action.detach)
