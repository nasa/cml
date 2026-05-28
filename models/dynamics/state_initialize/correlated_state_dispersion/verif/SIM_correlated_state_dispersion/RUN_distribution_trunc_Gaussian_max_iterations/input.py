exec(open("RUN_distribution_Gaussian/input.py").read())
# Test the "Limit too small for TruncatedGaussian" error in CorrelatedStateDispersion::generate_dispersions
verif.corr_state.dispersion_distribution = trick.CorrelatedStateDispersion.TruncatedGaussian
verif.corr_state.sigma_limit = 0.01

print(\
'\n****************************************************************************\n' \
'Error follows:\n' \
'Attempt to truncate the Gaussian fails within the specified max number of tries.\n' \
'Recovery is to switch to a Uniform distribution\n' \
'****************************************************************************\n')
