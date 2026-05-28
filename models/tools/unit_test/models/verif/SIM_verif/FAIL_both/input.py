#############  LOG DATA  #################
exec(open( "Log_data/log_data.py").read())
log_test_data( 1.0 )

# Insert specific initial conditions in the Unit_test files:
test.framework.data_file_name = "Unit_test_data/data.txt" 
test.framework.vars_file_name = "Unit_test_data/variables.txt" 

test.sweep_var1( 2.0, 3.1, 1.0)
test.sweep_var2( 5.0, 2.2 ,-1.0)
# Set the termination time.  The termination time should be the number of tests
# that you will perform.  in this case, the model will be tested under 4 sets
# of initial conditions.
trick.sim_services.exec_set_terminate_time(14)
