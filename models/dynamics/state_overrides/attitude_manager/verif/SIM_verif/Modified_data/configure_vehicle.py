dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_SinglePlanet
dynamics.dyn_manager_init.central_point_name = "Earth"

def bodyInit(veh, name):
    veh.integ_frame_name = "Earth.inertial"
    veh.set_name(name)
    veh.translational_dynamics = True
    veh.rotational_dynamics = False


bodyInit(test.body, "vehicleA")


# Mass
def massInit(init, veh, pos):
    init.set_subject_body(veh)
    init.properties.mass = 1.0
    init.properties.position = [0.0, 0.0, 0.0]
    init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
    init.properties.inertia = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
    init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
    init.properties.pt_orientation.trans = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
    init.allocate_points(1)
    mass_point = init.get_mass_point(0)
    mass_point.set_name("docking_port")
    mass_point.position[0] = pos[0]
    mass_point.position[1] = pos[1]
    mass_point.position[2] = pos[2]
    mass_point.pt_orientation.data_source = trick.Orientation.InputMatrix
    mass_point.pt_orientation.trans = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
    dynamics.dyn_manager.add_body_action(init)


massInit(test.mass_init, test.body, [7.5, 0.0, 0.0])


# State
def dynInit(trans, rot, veh, pos):
    trans.set_subject_body(veh)
    trans.body_frame_id = "composite_body"
    trans.reference_ref_frame_name = "Earth.inertial"
    trans.position = pos
    trans.velocity = [0.0, 10000.0, 0.0]
    dynamics.dyn_manager.add_body_action(trans)
    rot.set_subject_body(veh)
    rot.body_frame_id = "composite_body"
    rot.reference_ref_frame_name = "Earth.inertial"
    rot.orientation.data_source = trick.Orientation.InputEigenRotation
    rot.orientation.eigen_angle = 0.0
    rot.orientation.eigen_axis = [0.0, 0.0, 1.0]
    rot.ang_velocity = [0.001, 0.002, 0.002]
    dynamics.dyn_manager.add_body_action(rot)


dynInit(
    test.trans_init,
    test.rot_init,
    test.body,
    [7000000.0, 0.0, 0.0],
)

test.ref_frame.set_subject_name("vehicleA.composite_body")
test.ref_frame.set_planet_name("Earth")


test.att_manager.subscribe()

# Data logging
exec(open("Log_data/log_data.py").read())