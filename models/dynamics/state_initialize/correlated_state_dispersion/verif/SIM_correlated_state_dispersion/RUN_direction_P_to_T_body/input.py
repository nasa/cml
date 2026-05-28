# In this test, the attitude dispersions are specified in the body frame rather
# than in the inertial frame.
# The results should be compared with those form RUN_direction_P_to_T_ref on which
# this is based.

# The consequence should be that the T-mx dispersions should be transposed,
# and then shuffled according to the axis-mapping provided by the original T_mx
# (which represents inertial-to-body):
#     Inertial       Body
#        y            x
#        z            y
#        x            z
# So the dispersions applied to the first row when the covariance is in the
# inertial (REFERENCE) frame should show up in the 3rd column when the
# covariance is in the body frame.
# 2nd row should map to the 1st column
# 3rd row should map to the 2nd column
exec(open("RUN_direction_P_to_T_ref/input.py").read())
verif.corr_state.att_rot_defined = trick.CorrelatedStateDispersion.PerturbedToTrue
verif.corr_state.corr_att_frame = trick.CorrelatedStateDispersion.BODY_FRM
