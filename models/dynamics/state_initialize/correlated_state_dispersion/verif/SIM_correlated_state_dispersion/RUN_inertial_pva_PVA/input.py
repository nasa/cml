# Tests the correlated dispersion of pos, vel, and att

exec(open("Modified_data/common_input.py").read())
verif.disperse_pva = True
verif.corr_state.corr_option = trick.CorrelatedStateDispersion.CORRELATED_PV_ATT
log_att()
trick.stop(2)
