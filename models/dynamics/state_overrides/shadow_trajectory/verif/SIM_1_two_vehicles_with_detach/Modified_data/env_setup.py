exec(open("../Modified_data/common_env_setup.py").read())

veh_setup( vehicle.parent, "parent")
veh_setup( vehicle.child,  "child")
veh_setup( vehicle.shadow.body, "shadow")
# common-env-setup's veh_setup method switches trans-dynamics on;
# turn it off again for the shadow body only.
vehicle.shadow.body.translational_dynamics = False


mass_init( vehicle.mass_init[0], vehicle.parent)
mass_init( vehicle.mass_init[1], vehicle.child)
vehicle.mass_init[0].properties.position[0] = 1
vehicle.mass_init[1].properties.position[0] = -1
for ii in range (3):
  vehicle.mass_point[ii].pt_orientation.data_source =\
                                           trick.Orientation.InputQuaternion
vehicle.mass_point[0].set_name("parent_pt")
vehicle.mass_point[0].position[0] = 2
vehicle.mass_point[1].set_name("child_pt")
vehicle.mass_point[1].position[0] = -2
vehicle.mass_point[2].set_name("shadow_pt")
# Notes:
#   shadow_pt gets added to shadow.body in S_define:
#   shadow.body.add_mass_point( mass_point[2]);
#   The position of point[2] is deliberately not set and defaults to 0.

state_init( vehicle.parent)


# attach / detach:
vehicle.attach.set_subject_body( vehicle.child )
vehicle.attach.set_parent_body( vehicle.parent )
vehicle.attach.active = True
vehicle.attach.offset_pstr_cstr_pstr = [0,0,0]
vehicle.attach.pstr_cstr.data_source = trick.Orientation.InputMatrix
vehicle.attach.pstr_cstr.trans =  [[1,0,0],[0,1,0],[0,0,1]]

vehicle.detach.set_subject_body( vehicle.child )
vehicle.detach.active = False

dynamics.dyn_manager.add_body_action( vehicle.attach );
dynamics.dyn_manager.add_body_action( vehicle.detach );
