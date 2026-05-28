exec(open("RUN_distribution_Gaussian/input.py").read())
# Test the TruncatedGaussian method and whether the alternate name TruncatedNormal will have the same behaviour
verif.corr_state.dispersion_distribution = trick.CorrelatedStateDispersion.TruncatedNormal
# Test the "Invalid limit" warning in CorrelatedStateDispersion::generate_dispersions
verif.corr_state.sigma_limit = 0.0
print(\
'\n****************************************************************************\n' \
'Warning follows:\n' \
'Gaussian has been truncated to 0-sigma which is infinitesimally small\n' \
'Recovery is to default to 3-sigma\n' \
'****************************************************************************\n')
