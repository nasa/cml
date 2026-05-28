# Tests the mismatch -- setting the model to handle PVA
#                       but only passing P and V
# Should generate P,V,A dispersions but only apply P, V dispersions.

exec(open("Modified_data/common_input.py").read())

verif.corr_state.corr_option = trick.CorrelatedStateDispersion.CORRELATED_PV_ATT
verif.disperse_pv = True
log_att()
