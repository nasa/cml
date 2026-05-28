exec(open("RUN_inertial_pva_PVA/input.py").read())

verif.corr_state.dispersion_distribution = trick.CorrelatedStateDispersion.Normal
verif.corr_state.covariance[0][1] = 0
verif.corr_state.covariance[1][0] = 0

verif.corr_state.covariance[0][0] = 1
verif.corr_state.covariance[1][1] = 1
verif.corr_state.covariance[2][2] = 1
verif.corr_state.covariance[3][3] = 1
verif.corr_state.covariance[4][4] = 1
verif.corr_state.covariance[5][5] = 1
verif.corr_state.covariance[6][6] = 1
verif.corr_state.covariance[7][7] = 1
verif.corr_state.covariance[8][8] = 1

trick.stop(1)
