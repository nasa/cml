exec(open("RUN_frame_inertial/input.py").read())
verif.corr_state.pv_covar_frame = trick.CorrelatedStateDispersion.PV_COVAR_R_DEC_RA_V_FP_AZ

verif.corr_state.covariance[0][1] = 0.0
verif.corr_state.covariance[1][0] = 0.0
verif.corr_state.covariance[1][1] = 0.5/(6800000*6800000)
verif.corr_state.covariance[2][2] = 0.5/(6800000*6800000)
verif.corr_state.covariance[4][4] = 0.5/(8000*8000)
verif.corr_state.covariance[5][5] = 0.5/(8000*8000)
