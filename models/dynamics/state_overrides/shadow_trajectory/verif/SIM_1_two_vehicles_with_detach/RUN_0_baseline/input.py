'''
In this baseline run, the shadow-child picks up the trajectory-file-defined
state and runs alongside the child while the child is being carried along
with the parent, and the shadow-child is being driven by the same trajectory
data.
NOTE:
- In SIM_0_one_vehicle, commands were scheduled in the S_define.
  For this simulation, we try issuing those at the input file level.
- Must turn on the shadow before turning off the primary traj-model
'''
exec(open("Log_data/log_data.py").read())

exec(open("Modified_data/env_setup.py").read())
vehicle.traj.data_filename = "../data/traj"
vehicle.traj.input_file_in_pfix = False

# Don't care about rot state on either body.
# Note that setting override_*_state to false will switch on the
#   integrated state, so be sure to switch that off again.
vehicle.traj.set_override_rot_state(False)
vehicle.traj.set_add_rot_tables(False)
vehicle.traj.transition_rotational_dynamics = False

vehicle.shadow.traj.transition_rotational_dynamics = False
vehicle.shadow.traj.set_override_rot_state(False)
vehicle.shadow.traj.set_add_rot_tables(False)

trick.add_read(1, """
vehicle.shadow.subscribe()
vehicle.shadow_wrt_parent.subscribe()
vehicle.shadow_wrt_child.subscribe()
""")

trick.stop(8)
