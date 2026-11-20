exec(open("../Modified_data/common_env_setup.py").read())

veh_setup( vehicle.parent, "parent")
veh_setup( vehicle.child,  "child")

mass_init( vehicle.mass_init[0], vehicle.parent)
mass_init( vehicle.mass_init[1], vehicle.child)
vehicle.mass_init[0].properties.position[0] = 1
vehicle.mass_init[1].properties.position[0] = -1

state_init( vehicle.parent)


# attach / detach:
vehicle.attach.set_subject_body(vehicle.child)
vehicle.attach.set_parent_body(vehicle.parent)
vehicle.attach.active = True
vehicle.attach.offset_pstr_cstr_pstr = [0,0,0]
vehicle.attach.pstr_cstr.data_source = trick.Orientation.InputMatrix
vehicle.attach.pstr_cstr.trans =  [[1,0,0],[0,1,0],[0,0,1]]

vehicle.detach.set_subject_body(vehicle.child)
vehicle.detach.active = False

dynamics.dyn_manager.add_body_action( vehicle.attach );
dynamics.dyn_manager.add_body_action( vehicle.detach );
