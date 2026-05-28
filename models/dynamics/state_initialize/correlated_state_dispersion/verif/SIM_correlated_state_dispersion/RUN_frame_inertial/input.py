exec(open("Modified_data/common_input.py").read())
verif.disperse_pva = True
verif.true_position = [0.0, 6800000.0, 0.0]
verif.true_velocity = [8000.0, 0.0]
verif.corr_state.corr_option = trick.CorrelatedStateDispersion.CORRELATED_PV_ATT
verif.corr_state.pv_covar_frame = trick.CorrelatedStateDispersion.PV_COVAR_INRTL
log_att()
