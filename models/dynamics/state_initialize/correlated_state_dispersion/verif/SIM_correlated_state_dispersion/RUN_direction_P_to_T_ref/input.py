# This test serves as the baseline for the RUN_direction tests.
# The attitude dispersions are specified in the underlying reference frame

# The results should be compared with those from RUN_inertial_pva_PVA, on which
# this is based.
exec(open("RUN_inertial_pva_PVA/input.py").read())
verif.corr_state.att_rot_defined = trick.CorrelatedStateDispersion.PerturbedToTrue
verif.corr_state.corr_att_frame = trick.CorrelatedStateDispersion.REFERENCE_FRM
trick.stop(1)
