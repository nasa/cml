import math

dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_EmptySpace
dynamics.dyn_manager_init.central_point_name = "Space"

verif.trans_base.integ_frame_name = "Space.inertial"
verif.trans_base.set_name("trans_base")
verif.trans_base.translational_dynamics = False
verif.trans_base.rotational_dynamics = False

verif.rot_base.integ_frame_name = "Space.inertial"
verif.rot_base.set_name("rot_base")
verif.rot_base.translational_dynamics = False
verif.rot_base.rotational_dynamics = False

# Mass
def mass_init(init,veh):
  init.set_subject_body(veh)
  init.properties.mass = 1.0
  init.properties.position  = [0.0, 0.0, 0.0]
  init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
  init.properties.inertia = [[1,0,0],[0,1,0],[0,0,1]]
  init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
  init.properties.pt_orientation.trans = [[1,0,0],[0,1,0],[0,0,1]]
  dynamics.dyn_manager.add_body_action(init)

mass_init(verif.mass_init_trans_base, verif.trans_base)
mass_init(verif.mass_init_rot_base, verif.rot_base)

#State
verif.trans_init_trans_base.set_subject_body(verif.trans_base)
verif.trans_init_trans_base.body_frame_id = "composite_body"
verif.trans_init_trans_base.reference_ref_frame_name = "Space.inertial"
verif.trans_init_trans_base.position = [0, 0, 0]
verif.trans_init_trans_base.velocity = [0, 0, 0]
dynamics.dyn_manager.add_body_action(verif.trans_init_trans_base)

verif.rot_init_trans_base.set_subject_body(verif.trans_base)
verif.rot_init_trans_base.body_frame_id = "composite_body"
verif.rot_init_trans_base.reference_ref_frame_name = "Space.inertial"
verif.rot_init_trans_base.orientation.data_source  = trick.Orientation.InputQuaternion
verif.rot_init_trans_base.ang_velocity             = [0, 0, 0]
dynamics.dyn_manager.add_body_action(verif.rot_init_trans_base)

verif.trans_init_rot_base.set_subject_body(verif.rot_base)
verif.trans_init_rot_base.body_frame_id = "composite_body"
verif.trans_init_rot_base.reference_ref_frame_name = "Space.inertial"
verif.trans_init_rot_base.position = [0, 0, 0]
verif.trans_init_rot_base.velocity = [0, 0, 0]
dynamics.dyn_manager.add_body_action(verif.trans_init_rot_base)

verif.rot_init_rot_base.set_subject_body(verif.rot_base)
verif.rot_init_rot_base.body_frame_id = "composite_body"
verif.rot_init_rot_base.reference_ref_frame_name = "Space.inertial"
verif.rot_init_rot_base.orientation.data_source  = trick.Orientation.InputEigenRotation
verif.rot_init_rot_base.orientation.eigen_angle  = 0.0 #math.pi
verif.rot_init_rot_base.orientation.eigen_axis   = [0, 0, 0]
verif.rot_init_rot_base.ang_velocity             = [0, 0, 0]
dynamics.dyn_manager.add_body_action(verif.rot_init_rot_base)

verif.framework.enabled = False