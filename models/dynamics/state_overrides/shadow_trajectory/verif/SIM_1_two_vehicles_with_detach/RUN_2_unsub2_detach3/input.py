'''
 Now detach the child from the parent.
 The child will start to follow an integrated trajectory (+2 per second) while
 the shadow-child will continue to follow the prescribed trajectory.
- The child's state will "pause" because the state propagation has already been
  carried out up to this frame, and the detach of the child means its state
  will not be updated by being dragged along by the parent when the parent is
  updated to the prescribed trajectory. THIS IS A REAL CONSEQUENCE OF POOR
  SCHEDULING AND IS INCLUDED HERE FOR ILLUSTRATIVE PURPOSES.
- The parent's core-body and structure-frame will "pause" because the
  composite-body frame (specified by the prescribed trajectory) jumps forward by
  +1 at detach so when the prescribed-trajectory updates, it updates its
  position to its post-detach position. This is a consequence of mass-motion
  and is not an error in the dynamic propagation.
'''
exec(open("RUN_0_baseline/input.py").read())

vehicle.traj.update_before_deactivating = True
trick.add_read(2, """
vehicle.traj.unsubscribe()
""")

trick.add_read(3, """
vehicle.detach.active = True
vehicle.child_wrt_parent.subscribe()
""")
