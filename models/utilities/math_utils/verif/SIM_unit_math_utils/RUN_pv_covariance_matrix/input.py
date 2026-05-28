exec(open("Log_data/log_math_utils.py").read())
log_pv_covariance_matrix()

mutil_verif.framework.data_file_name = "Unit_test/matrix_pv_cov_data.txt"
mutil_verif.framework.vars_file_name = "Unit_test/matrix_pv_cov_variables.txt"

mutil_verif.test_pv_covariance_matrix = True

trick.stop(6)