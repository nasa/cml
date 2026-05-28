exec(open( "Log_data/log_data.py").read())

atmos_test_verif.framework.data_file_name = "Unit_test_data/data2.txt"
atmos_test_verif.framework.vars_file_name = "Unit_test_data/variables.txt"
atmos_test_verif.atmos_test.subscribe_complete_calculation()
atmos_test_verif.atmos_test.terminate_on_nan_euler = 1

trick.stop(7)
