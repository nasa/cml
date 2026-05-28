exec(open("Log_data/log_math_utils.py").read())
log_unit_vec_deriv()
mutil_verif.test_unit_vec_deriv = True

mutil_verif.framework.data_file_name = "Unit_test/vector_unit_deriv_data.txt"
mutil_verif.framework.vars_file_name = "Unit_test/vector_variables.txt"

print("\n"
"*********************************************************************\n"
"Error message x2: vector is a zero-vector; no unit vector definable.\n"
"*********************************************************************\n")

trick.stop(10)
