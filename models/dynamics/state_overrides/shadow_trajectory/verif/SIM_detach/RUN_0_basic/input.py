# In this baseline run, the shadow-child picks up the trajectory-file-defined
# state.  Note that this is the state of the combined CoM, so the shadow-child
# gets a state that sits at the midpoint between the real-child and the parent
# bodies. The parent and child remain attached, and continue to follow the
# prescribed trajectory.
exec(open("Log_data/log_data.py").read())

exec(open("Modified_data/env_setup.py").read())
vehicle.traj.data_filename = "../data/traj"
vehicle.traj.input_file_in_pfix = False

#Don't care about rot state on either body
vehicle.traj.set_override_rot_state(False)
vehicle.traj.set_add_rot_tables(False)

vehicle.shadow.traj.set_override_rot_state(False)
vehicle.shadow.traj.set_add_rot_tables(False)

trick.stop(6)
