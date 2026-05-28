# Purpose: "nominal" scenario

exec(open("Log_data/log_verif.py").read())
log_group1_verif(1.0)
log_group2_verif(1.0)

trick_logging_verif.input1.data_file_name = "unit_test/data.txt"
trick_logging_verif.input1.vars_file_name = "unit_test/vars.txt"

trick.sim_services.exec_set_terminate_time(6.0)
