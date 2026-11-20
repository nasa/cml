# In this run, the shadow-child still picks up the trajectory-file-defined
# state.  In this run, the parent and child bodies separate and their
# state evolution transition to an integrated state.
exec(open("RUN_0_basic/input.py").read())
vehicle.detach_parent_child = True
