# Dynamics setup
dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_EmptySpace
dynamics.dyn_manager_init.central_point_name = "Space"

ventset_obj.root_body.integ_frame_name = "Space.inertial"
ventset_obj.root_body.set_name("root_body")
ventset_obj.root_body.translational_dynamics = True
ventset_obj.root_body.rotational_dynamics = True

ventset_obj.vent_body.integ_frame_name = "Space.inertial"
ventset_obj.vent_body.set_name("vent_body")
ventset_obj.vent_body.translational_dynamics = True
ventset_obj.vent_body.rotational_dynamics = True

# Vent locations
ventset_obj.set_2_tanks.get_vent(0).location[0] = -1.0 # m
ventset_obj.set_2_tanks.get_vent(0).location[1] = 0.0 # m
ventset_obj.set_2_tanks.get_vent(0).location[2] = 0.0 # m

ventset_obj.set_2_tanks.get_vent(1).location[0] = 0.0 # m
ventset_obj.set_2_tanks.get_vent(1).location[1] = -1.0 # m
ventset_obj.set_2_tanks.get_vent(1).location[2] = 0.0 # m

ventset_obj.extra_vent.location[0] = 1.0 # m
ventset_obj.extra_vent.location[1] = 1.0 # m
ventset_obj.extra_vent.location[2] = 0.0 # m

ventset_obj.extra_simple_vent.location[0] = -1.0 # m
ventset_obj.extra_simple_vent.location[1] =  0.0 # m
ventset_obj.extra_simple_vent.location[2] = -1.0 # m

# Body initialization
def mass_init(init, veh):
  init.set_subject_body(veh)
  init.properties.mass = 1.0
  init.properties.position = [0,0,0]
  init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
  init.properties.inertia = [[1,0,0],[0,1,0],[0,0,1]]
  init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
  init.properties.pt_orientation.trans = [[1,0,0],[0,1,0],[0,0,1]]
  dynamics.dyn_manager.add_body_action(init)

mass_init(ventset_obj.vent_massinit, ventset_obj.vent_body)
mass_init(ventset_obj.root_massinit, ventset_obj.root_body)

# State initialization
ventset_obj.trans_init.set_subject_body(ventset_obj.vent_body)
ventset_obj.trans_init.body_frame_id = "composite_body"
ventset_obj.trans_init.reference_ref_frame_name = "Space.inertial"
ventset_obj.trans_init.position = [0.0, 0.0, 0.0]
ventset_obj.trans_init.velocity = [0.0, 0.0, 0.0]
dynamics.dyn_manager.add_body_action(ventset_obj.trans_init)

ventset_obj.rot_init.set_subject_body(ventset_obj.vent_body)
ventset_obj.rot_init.body_frame_id = "composite_body"
ventset_obj.rot_init.reference_ref_frame_name = "Space.inertial"
ventset_obj.rot_init.orientation.data_source  = trick.Orientation.InputEigenRotation
ventset_obj.rot_init.orientation.eigen_angle  = 0.0
ventset_obj.rot_init.orientation.eigen_axis   = [0.0, 0.0, 1.0]
ventset_obj.rot_init.ang_velocity = [0.0, 0.0, 0.0]
dynamics.dyn_manager.add_body_action(ventset_obj.rot_init)


# Attach tanks to vent_body
for ii in range(2):
  ventset_obj.tank[ii].set_name("Tank" + str(ii))
  ventset_obj.tank[ii].dynamic_properties.consumable_mass = 1.0 # kg
  ventset_obj.tank[ii].residual_mass = 0.5 # kg
  ventset_obj.tank[ii].initialize_mass(ventset_obj.vent_massinit.properties, ventset_obj.vent_massinit.points)
  ventset_obj.tank[ii].initialize_dyn_mass()
  ventset_obj.tank[ii].update_mass()

  ventset_obj.attach_tank[ii].set_parent_body(ventset_obj.vent_body)
  ventset_obj.attach_tank[ii].set_subject_body(ventset_obj.tank[ii])
  ventset_obj.attach_tank[ii].pstr_cstr.data_source = trick.Orientation.InputQuaternion
  # Relative position remains at 0, relative orientation remains at identity
  ventset_obj.attach_tank[ii].active = True
  dynamics.dyn_manager.add_body_action(ventset_obj.attach_tank[ii])

# attach vent_body to root_body
ventset_obj.attach.set_parent_body(ventset_obj.root_body)
ventset_obj.attach.set_subject_body(ventset_obj.vent_body)
#Relative position remains at 0, relative orientation remains at identity
ventset_obj.attach.pstr_cstr.data_source = trick.Orientation.InputQuaternion
ventset_obj.attach.active = True
dynamics.dyn_manager.add_body_action(ventset_obj.attach)

exec(open("Modified_data/vent_properties.py").read())

ventset_obj.set_2_tanks.subscribe()
trick.stop(10.0)
