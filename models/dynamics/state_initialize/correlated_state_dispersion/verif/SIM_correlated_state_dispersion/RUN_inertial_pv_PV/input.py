# Tests the correlated dispersion of pos, and vel, but not att

exec(open("Modified_data/common_input.py").read())
verif.corr_state.corr_option = trick.CorrelatedStateDispersion.CORRELATED_PV
verif.disperse_pv = True
log_att()
