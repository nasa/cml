dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_EmptySpace
dynamics.dyn_manager_init.central_point_name = "Space"

contact_obj.ref.integ_frame_name = "Space.inertial"
contact_obj.ref.set_name("refbody")
contact_obj.ref.translational_dynamics = False
contact_obj.ref.rotational_dynamics = False

contact_obj.override.integ_frame_name = "Space.inertial"
contact_obj.override.set_name("overridebody")
contact_obj.override.translational_dynamics = False
contact_obj.override.rotational_dynamics = False

contact_obj.root.integ_frame_name = "Space.inertial"
contact_obj.root.set_name("rootbody")
contact_obj.root.translational_dynamics = False
contact_obj.root.rotational_dynamics = False

# Mass
def mass_init(init, veh):
  init.set_subject_body(veh)
  init.properties.mass = 1.0
  init.properties.position  = [0.0, 0.0, 0.0]
  init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
  init.properties.inertia = [[1,0,0],[0,1,0],[0,0,1]]
  init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
  init.properties.pt_orientation.trans = [[1,0,0],[0,1,0],[0,0,1]]
  dynamics.dyn_manager.add_body_action(init)

mass_init( contact_obj.mass_init_ref,
           contact_obj.ref)
mass_init( contact_obj.mass_init_override,
           contact_obj.override)
mass_init( contact_obj.mass_init_root,
           contact_obj.root)

# State
contact_obj.trans_init.set_subject_body(contact_obj.ref)
contact_obj.trans_init.body_frame_id = "structure"
contact_obj.trans_init.reference_ref_frame_name = "Space.inertial"
contact_obj.trans_init.position = [0.0, 0.0, 0.0]
contact_obj.trans_init.velocity = [0.0, 0.0, 0.0]
dynamics.dyn_manager.add_body_action( contact_obj.trans_init)

contact_obj.rot_init.set_subject_body(contact_obj.ref)
contact_obj.rot_init.body_frame_id = "structure"
contact_obj.rot_init.reference_ref_frame_name = "Space.inertial"
contact_obj.rot_init.orientation.data_source  = trick.Orientation.InputQuaternion
dynamics.dyn_manager.add_body_action( contact_obj.rot_init)

# Attach
contact_obj.attach_root_to_ref.set_parent_body(contact_obj.ref)
contact_obj.attach_root_to_ref.set_subject_body(contact_obj.root)
contact_obj.attach_root_to_ref.offset_pstr_cstr_pstr = [1.0, 2.0, 0.0]
contact_obj.attach_root_to_ref.pstr_cstr.data_source = trick.Orientation.InputMatrix
contact_obj.attach_root_to_ref.pstr_cstr.trans = [[-1,0,0],[0,-1,0],[0,0,1]]
contact_obj.attach_root_to_ref.active = True
dynamics.dyn_manager.add_body_action(contact_obj.attach_root_to_ref)

contact_obj.attach_override_to_root.set_parent_body(contact_obj.root)
contact_obj.attach_override_to_root.set_subject_body(contact_obj.override)
contact_obj.attach_override_to_root.offset_pstr_cstr_pstr = [0.0, 2.0, 0.0]
contact_obj.attach_override_to_root.pstr_cstr.data_source = trick.Orientation.InputMatrix
contact_obj.attach_override_to_root.pstr_cstr.trans = [[1,0,0],[0,-1,0],[0,0,-1]]
contact_obj.attach_override_to_root.active = True
dynamics.dyn_manager.add_body_action(contact_obj.attach_override_to_root)

# Data logging
exec(open("Log_data/log_data.py").read())

contact_obj.contact.subscribe()
contact_obj.contact.set_deactivation_threshold(1.0)

trick.stop(6.8)
