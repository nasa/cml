'''
 Issuing commands via add_read() means that command to deactivate the
 primary traj model will be issued at t=4 at the top of the
 frame, before the trajectory model has updated for that specified time.
 Make sure the update_before_deactivating flag is back to its default true.
'''
exec(open("RUN_1_detach3/input.py").read())
vehicle.traj.update_before_deactivating = True
trick.add_read(4, """
vehicle.traj.unsubscribe()
""")
