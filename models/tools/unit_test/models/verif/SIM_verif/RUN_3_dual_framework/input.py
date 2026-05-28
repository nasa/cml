#############  LOG DATA  #################
exec(open( "Log_data/log_data.py").read())
log_test_data( 1.0 )

# Insert specific initial conditions in the Unit_test files:
test.framework.data_file_name = "Unit_test_data/var1_data.txt" 
test.framework.vars_file_name = "Unit_test_data/var1.txt" 
test.another_framework.data_file_name = "Unit_test_data/var2_data.txt" 
test.another_framework.vars_file_name = "Unit_test_data/var2.txt" 

test.framework.cycle_data = True

# Set the termination time.  The termination time should be the number of tests
# that you will perform.  in this case, the model will be tested under 4 sets
# of initial conditions.
trick.sim_services.exec_set_terminate_time(14)
