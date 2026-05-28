exec(open("Log_data/log_data.py").read())
log_test_data( 1.0 )

# Insert specific initial conditions in the Unit_test files:
double_words.framework.data_file_name = "Unit_test_data_warn/data.txt"
double_words.framework.vars_file_name = "Unit_test_data_warn/variables.txt"

trick.sim_services.exec_set_terminate_time(4)
