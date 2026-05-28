exec(open("Log_data/log_data.py").read())

exec(open("Modified_data/env_setup.py").read())

# Insert specific initial conditions in the Unit_test files:
test.framework.data_file_name = "Unit_test_data/data.txt"
test.framework.vars_file_name = "Unit_test_data/variables.txt"

test.gravity_manager.subscribe()

# Set the termination time.  The termination time should be the number of tests
# that you will perform.  in this case, the model will be tested under 4 sets
# of initial conditions.
trick.sim_services.exec_set_terminate_time(14)
