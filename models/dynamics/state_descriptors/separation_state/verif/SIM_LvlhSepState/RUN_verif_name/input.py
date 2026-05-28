dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_SinglePlanet
dynamics.dyn_manager_init.central_point_name = "Earth"

#############  LOG DATA  #################
exec(open( "Log_data/log_data.py").read())

test_sep.source_init.name = "test-point"
test_sep.source_init.pt_orientation.data_source = trick.Orientation.InputQuaternion
test_sep.source_init.position = [0,0.5,0]


test_sep.subject_init.name = "test-point"
test_sep.subject_init.pt_orientation.data_source = trick.Orientation.InputQuaternion
test_sep.subject_init.position = [0,0,-0.7]

test_sep.source.integ_frame_name = "Earth.inertial"
test_sep.source.set_name("source")
test_sep.subject.integ_frame_name = "Earth.inertial"
test_sep.subject.set_name("subject")

# Turn off dynamics
test_sep.source.translational_dynamics = False
test_sep.source.rotational_dynamics = False
test_sep.subject.translational_dynamics = False
test_sep.subject.rotational_dynamics = False

# set state of source frame
test_sep.source.composite_body.state.trans.position = [7000000,0,0]
test_sep.source.composite_body.state.trans.velocity = [0,1,0]
# This makes the LVLH <-> inertial frame relationship:
#    LVLH      Inertial
#     x            y
#     y           -z
#     z           -x

# The source body is left at its default position, velocity, and angular rates
# (zero) and default orientation (identity).
# The subject body is given a different state every time-step by the unit-test
# framework.
test_sep.framework.data_file_name = "Unit_test_data/data.txt"
test_sep.framework.vars_file_name = "Unit_test_data/variables.txt"

test_sep.sep_state_1.rel_orientation.euler_sequence = trick.Orientation.EulerXYZ
test_sep.sep_state_2.rel_orientation.euler_sequence = trick.Orientation.EulerXYZ

trick.add_read(0,"test_sep.source.propagate_state()")


# Call subscribe and unsubscribe methods at specified times
trick.add_read(1.0, """
test_sep.sep_state_1.subscribe()
test_sep.sep_state_2.subscribe()
""")
trick.add_read(8.0, """
test_sep.sep_state_1.unsubscribe()
test_sep.sep_state_2.unsubscribe()
""")

# Set the termination time.  The termination time should be the number of tests
# that you will perform minus one (because the first test is at t = 0).  In this
# case, the model will be tested under 8 sets of conditions.
trick.stop(8.0)
