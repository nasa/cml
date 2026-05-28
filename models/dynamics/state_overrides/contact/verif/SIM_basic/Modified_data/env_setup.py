import math

dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_EmptySpace
dynamics.dyn_manager_init.central_point_name = "Space"

contact_obj.ref.integ_frame_name = "Space.inertial"
contact_obj.ref.set_name("reference")
contact_obj.ref.translational_dynamics = False
contact_obj.ref.rotational_dynamics = False

contact_obj.override.integ_frame_name = "Space.inertial"
contact_obj.override.set_name("override")
contact_obj.override.translational_dynamics = False
contact_obj.override.rotational_dynamics = False

contact_obj.contact.set_deactivation_threshold(1.0)

# Mass
def mass_init(init,veh):
  init.set_subject_body(veh)
  init.properties.mass = 1.0
  init.properties.position  = [0.0, -5.0, 0.0]
  init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
  init.properties.inertia = [[1,0,0],[0,1,0],[0,0,1]]
  init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
  init.properties.pt_orientation.trans = [[-1,0,0],[0,1,0],[0,0,-1]]
  dynamics.dyn_manager.add_body_action(init)

mass_init(contact_obj.mass_init_ref, contact_obj.ref)
mass_init(contact_obj.mass_init_override, contact_obj.override)

#State
contact_obj.trans_init_ref.set_subject_body(contact_obj.ref)
contact_obj.trans_init_ref.body_frame_id = "composite_body"
contact_obj.trans_init_ref.reference_ref_frame_name = "Space.inertial"
contact_obj.trans_init_ref.position = [0, 0, 0]
contact_obj.trans_init_ref.velocity = [0, 0, 0]
dynamics.dyn_manager.add_body_action(contact_obj.trans_init_ref)

contact_obj.rot_init_ref.set_subject_body(contact_obj.ref)
contact_obj.rot_init_ref.body_frame_id = "composite_body"
contact_obj.rot_init_ref.reference_ref_frame_name = "Space.inertial"
contact_obj.rot_init_ref.orientation.data_source  = trick.Orientation.InputQuaternion
contact_obj.rot_init_ref.ang_velocity             = [0, 0, 0]
dynamics.dyn_manager.add_body_action(contact_obj.rot_init_ref)

contact_obj.trans_init_override.set_subject_body(contact_obj.override)
contact_obj.trans_init_override.body_frame_id = "composite_body"
contact_obj.trans_init_override.reference_ref_frame_name = "Space.inertial"
contact_obj.trans_init_override.position = [1, 0, 0]
contact_obj.trans_init_override.velocity = [0, 0, 0]
dynamics.dyn_manager.add_body_action(contact_obj.trans_init_override)

contact_obj.rot_init_override.set_subject_body(contact_obj.override)
contact_obj.rot_init_override.body_frame_id = "composite_body"
contact_obj.rot_init_override.reference_ref_frame_name = "Space.inertial"
contact_obj.rot_init_override.orientation.data_source  = trick.Orientation.InputEigenRotation
contact_obj.rot_init_override.orientation.eigen_angle  = math.pi
contact_obj.rot_init_override.orientation.eigen_axis   = [1, 0, 0]
contact_obj.rot_init_override.ang_velocity             = [0, 0, 0]
dynamics.dyn_manager.add_body_action(contact_obj.rot_init_override)

contact_obj.contact.subscribe()

# Data logging
exec(open("Log_data/log_data.py").read())
