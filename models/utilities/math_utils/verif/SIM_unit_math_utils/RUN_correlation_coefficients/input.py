exec(open("Log_data/log_math_utils.py").read())
log_correlation_coefficients()
trick.CMLMessage.set_publish_level (trick.CMLMessage.Inform)

mutil_verif.framework.data_file_name = "Unit_test/matrix_corr_coeff_data.txt"
mutil_verif.framework.vars_file_name = "Unit_test/matrix_corr_coeff_variables.txt"

mutil_verif.test_correlation_coefficients = True

trick.stop(7)
