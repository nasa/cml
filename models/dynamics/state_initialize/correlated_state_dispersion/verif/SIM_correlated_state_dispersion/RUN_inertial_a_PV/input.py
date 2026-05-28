# Tests the 1 argument call to disperse_state
#  - P, V, A dispersions will be generated
#  - Only A dispersions will be applied.

# NOTE  - setting corr_option to PV to confirm that this has no effect.
#         A dispersion should still be generated and applied.
exec(open("Modified_data/common_input.py").read())

verif.corr_state.corr_option = trick.CorrelatedStateDispersion.CORRELATED_PV
verif.disperse_a = True
log_att()
