exec(open("RUN_0_default/input.py").read())

#Don't care about rot state on either body
vehicle.traj.set_override_rot_state(False)
vehicle.traj.set_add_rot_tables(False)
