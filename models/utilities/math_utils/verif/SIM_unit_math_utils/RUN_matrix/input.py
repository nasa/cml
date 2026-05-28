exec(open("Log_data/log_math_utils.py").read())
log_matrices()

mutil_verif.framework.data_file_name = "Unit_test/matrix_algebra_data.txt"
mutil_verif.framework.vars_file_name = "Unit_test/matrix_algebra_variables.txt"

mutil_verif.test_matrices = True

print("\n"
"************************************************************************\n"
"Error -- Invalid starting indices for submatrix extraction. \n"
"         Cannot extract submatrix. \n"
"************************************************************************")

print("\n"
"************************************************************************\n"
"Error -- Invalid starting indices for submatrix insertion. \n"
"         Cannot insert submatrix. \n"
"************************************************************************")

print("\n"
"************************************************************************\n"
"Error -- Invalid matrix for inverse computations\n"
"         cholesky decomposition fails, followed by inverse computation\n"
"************************************************************************")

trick.stop(4)
