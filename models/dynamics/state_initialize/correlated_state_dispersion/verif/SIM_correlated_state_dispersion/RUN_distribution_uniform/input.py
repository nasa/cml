exec(open("RUN_distribution_Gaussian/input.py").read())
verif.corr_state.dispersion_distribution = trick.CorrelatedStateDispersion.Uniform
# Test the "Negative limit" warning in CorrelatedStateDispersion::generate_dispersions
verif.corr_state.sigma_limit = -2.0
print(\
'\n****************************************************************************\n' \
'Warning follows:\n' \
'Negative sigma-limit will be modified to its absolute value.\n' \
'****************************************************************************\n')
