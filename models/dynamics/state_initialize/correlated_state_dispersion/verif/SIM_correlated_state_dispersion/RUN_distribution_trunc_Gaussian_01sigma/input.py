exec(open("RUN_distribution_Gaussian/input.py").read())
# Test the TruncatedGaussian method and whether the alternate name TruncatedNormal will have the same behaviour
verif.corr_state.dispersion_distribution = trick.CorrelatedStateDispersion.TruncatedGaussian
verif.corr_state.sigma_limit = 0.1
