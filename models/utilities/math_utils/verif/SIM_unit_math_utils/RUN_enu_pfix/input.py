exec(open("Log_data/log_math_utils.py").read())
log_Q_enu_to_pfix()

mutil_verif.framework.data_file_name = "Unit_test/Q_enu_to_pfix_data.txt"
mutil_verif.framework.vars_file_name = "Unit_test/Q_enu_to_pfix_variables.txt"

mutil_verif.test_Q_enu_to_pfix = True

trick.sim_services.exec_set_terminate_time(17)
