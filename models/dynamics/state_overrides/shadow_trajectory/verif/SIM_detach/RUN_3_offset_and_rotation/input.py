# In this run, the initial state of the vehicle is given a rotation on the
# z-axis.  Because the traj model does not overwrite rotational state, this
# state is retained, but ignored.
# When the vehicles separate, this rotation will produce opposing y-axis
# velocities on the parent and child bodies.
# The trajectory data does not know anything of this off-nominal state, so
# it will continue to apply zeros to the y-axis of the shadow state.
exec(open("RUN_2_with_offset/input.py").read())
add_y_axis_log(dr_group)

vehicle.state_initialize.yaw_rate_body = 0.5
