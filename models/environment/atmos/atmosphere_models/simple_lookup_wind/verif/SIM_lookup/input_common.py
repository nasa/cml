exec(open( "Log_data/log_test.py").read())

test.framework.data_file_name = "Unit_test/data.txt"
test.framework.vars_file_name = "Unit_test/variables.txt"

trick.sim_services.exec_set_terminate_time(5.0)
