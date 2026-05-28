exec(open("RUN_inertial_pv_PV/input.py").read())
# This will activate a function in S_define that passes null pointers instead
# of position and velocity, triggering the error() in
# CorrelatedStateDispersion::transform_translational_dispersions
print(\
'\n****************************************************************************\n' \
'3 Errors follow, all the same:\n' \
'Attempt to run the model with NULL pointers for the state will fail:\n' \
'1. position is NULL\n' \
'2: velocity is NULL\n' \
'3: position and velocity are NULL.\n'
'****************************************************************************\n')
trick.add_read(0, "verif.null_check()")
trick.stop(0)
