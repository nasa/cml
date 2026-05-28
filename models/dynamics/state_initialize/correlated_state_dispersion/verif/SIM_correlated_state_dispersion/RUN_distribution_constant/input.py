exec(open("RUN_distribution_Gaussian/input.py").read())
# Test the Constant dispersion option
verif.corr_state.dispersion_distribution = trick.CorrelatedStateDispersion.Constant
verif.corr_state.sigma_limit = 2.0
