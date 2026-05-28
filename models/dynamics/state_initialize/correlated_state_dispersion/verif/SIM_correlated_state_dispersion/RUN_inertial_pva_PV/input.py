# Tests the mismatch -- setting the model to handle PV
#                       but then passing P, V, and A
# Should generate P,V dispersions only; application of 0 dispersion on A will
# leave A at nominal.

exec(open("Modified_data/common_input.py").read())
verif.corr_state.corr_option = trick.CorrelatedStateDispersion.CORRELATED_PV
verif.disperse_pva = True
log_att()
