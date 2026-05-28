# Tests the NoDispersion option, which causes the dispersion model to exit
# without doing anything.
exec(open("RUN_distribution_Gaussian/input.py").read())
verif.corr_state.dispersion_distribution = trick.CorrelatedStateDispersion.NoDispersion
