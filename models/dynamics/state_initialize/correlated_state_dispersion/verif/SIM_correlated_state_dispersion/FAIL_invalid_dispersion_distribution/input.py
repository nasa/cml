# Test the "Invalid Dispersion Distribution" fail() in
# CorrelatedStateDispersion::generate_dispersions
exec(open("Modified_data/common_input.py").read())
verif.disperse_pv = True
verif.corr_state.dispersion_distribution = 10
