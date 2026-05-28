exec(open("frame_transformation_common_input.py").read())
log_inertial_to_lvlh()
log_inertial_to_uvw()
log_inertial_to_reference()
log_T_pfix_to_enu()

mutil_verif.test_T_NULL = True
trick.stop(0)
