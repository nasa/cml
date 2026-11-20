'''
 Repeat of RUN_3, illustrating the significance of the
 update_before_deactivating flag.
 Because the command to deactivate the primary traj model is issued at t=4
 at the top of the frame (before the trajectory model has updated), without the
 update_before_deactivating flag, the state going to integration will be that
 from t=3, not t=4. By the time the traj-model deactivates, state integration
 from t=3 to t=4 is already complete so the state at  t=4 is neither integrated
 from t=3 nor overwritten by the traj-model.
'''
exec(open("RUN_3_detach3_unsub4_wait/input.py").read())
vehicle.traj.update_before_deactivating = False
