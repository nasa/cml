exec(open("RUN_direction_P_to_T_ref/input.py").read())
verif.corr_state.att_rot_defined = trick.cml.CorrelatedStateDispersion.TrueToPerturbed
verif.corr_state.corr_att_frame = trick.cml.CorrelatedStateDispersion.REFERENCE_FRM
