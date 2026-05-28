# Trigger the "Invalid covariance frame" fail() in
# CorrelatedStateDispersion::transform_translational_dispersions
exec(open("Modified_data/common_input.py").read())
verif.disperse_pv = True
verif.corr_state.pv_covar_frame = 10
