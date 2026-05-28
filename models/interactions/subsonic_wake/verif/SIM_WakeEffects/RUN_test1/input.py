exec(open("Log_data/wake_effects_unittest.py").read())
log_add_instance( dr_group, "test.wake_no_force")

test.wake_no_force.subscribe();
test.wake_with_force.subscribe();

test.framework.data_file_name = "Unit_test_data/test1_data.txt"
test.framework.vars_file_name = "Unit_test_data/test1_vars.txt"

trick.stop(5)
