exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_data.py").read())

verif.true_position = [6800000.0, 0.0, 0.0]
verif.true_velocity = [0.0, 8000.0, 0.0]
verif.true_T_mx = [[0,1,0],[0,0,1],[1,0,0]]

verif.corr_state.dispersion_distribution = trick.CorrelatedStateDispersion.Gaussian
verif.corr_state.sigma_limit = 3.0
verif.corr_state.pv_covar_frame = trick.CorrelatedStateDispersion.PV_COVAR_INRTL

verif.corr_state.covariance[0][0] = 100
verif.corr_state.covariance[1][1] = 1
verif.corr_state.covariance[2][2] = 0.01

verif.corr_state.covariance[3][3] = 100
verif.corr_state.covariance[4][4] = 1
verif.corr_state.covariance[5][5] = 0.01

verif.corr_state.covariance[6][6] = 100
verif.corr_state.covariance[7][7] = 1
verif.corr_state.covariance[8][8] = 0.01

verif.corr_state.covariance[0][1] = 9
verif.corr_state.covariance[1][0] = 9



verif.planet.name = "Placeholder"
trick.stop(1)
