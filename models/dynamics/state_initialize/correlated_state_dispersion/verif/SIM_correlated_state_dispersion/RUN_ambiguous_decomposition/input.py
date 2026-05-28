# Shows what happens if the covariance matrix has a determinant of 0. When the
# MathUtils::cholesky_decomposition function is called in
# CorrelatedStateDispersion::generate_dispersions, it will print an
# "Ambiguous Decomposition" warning.
exec(open("Modified_data/common_input.py").read())
verif.disperse_pv = True

verif.corr_state.covariance[0][0] = 1
verif.corr_state.covariance[1][0] = 1
verif.corr_state.covariance[0][1] = 1
verif.corr_state.covariance[1][1] = 1
