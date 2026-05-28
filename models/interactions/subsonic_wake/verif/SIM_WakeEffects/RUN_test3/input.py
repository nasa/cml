exec(open("Log_data/wake_effects_unittest.py").read())

test.wake_no_force.subscribe();
test.wake_with_force.subscribe();

test.bodyB_inertial_vel[0] = 10
test.set_attitude_by_attack = True

test.framework.data_file_name = "Unit_test_data/test3_data.txt"
test.framework.vars_file_name = "Unit_test_data/test3_vars.txt"

trick.stop(7)
