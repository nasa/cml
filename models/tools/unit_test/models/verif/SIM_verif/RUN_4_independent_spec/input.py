#############  LOG DATA  #################
exec(open( "Log_data/log_data.py").read())
log_test_data( 1.0 )

# Insert specific initial conditions in the Unit_test files:
test.framework.linked_vars_file_name = "Unit_test_data/test_4_variables.txt" 

#disable the other framework, it is not needed for this test case
test.another_framework.enabled = False

# Set the termination time.  The termination time should be the number of tests
# that you will perform.  in this case, the model will be tested under 4 sets
# of initial conditions.
trick.sim_services.exec_set_terminate_time(14)
