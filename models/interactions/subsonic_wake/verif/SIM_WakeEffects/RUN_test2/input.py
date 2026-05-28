exec(open("Log_data/wake_effects_unittest.py").read())

test.wake_no_force.subscribe();
test.wake_with_force.subscribe();

test.bodyB_inertial_vel[0] = 10

test.framework.data_file_name = "Unit_test_data/test2_data.txt"
test.framework.vars_file_name = "Unit_test_data/test2_vars.txt"

trick.stop(10)
