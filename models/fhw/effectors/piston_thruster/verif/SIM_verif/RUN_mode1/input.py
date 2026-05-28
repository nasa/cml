#############  LOG DATA  #################
exec(open( "Log_data/log_data.py").read())
log_test_data( 1.0 )

# Insert specific initial conditions in the Unit_test files:
pistons_unit_test.framework.data_file_name = "Unit_test_data/data.txt" 
pistons_unit_test.framework.vars_file_name = "Unit_test_data/variables.txt" 

pistons_unit_test.pistons.subscribe()

trick.sim_services.exec_set_terminate_time(12)
