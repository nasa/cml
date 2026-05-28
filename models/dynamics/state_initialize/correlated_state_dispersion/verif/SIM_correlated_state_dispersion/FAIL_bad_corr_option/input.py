# This will trigger the fail() in CorrelatedStateDispersion::disperse_state
exec(open("Modified_data/common_input.py").read())
verif.disperse_pv = True
verif.corr_state.corr_option = 5
