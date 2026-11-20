# In this run, the initial offset of the child relative to the combined CoM is
# factored in when computing the state of the shadow-child from the trajectory
# file.  [Recall, the trajectory file specifies the state of the combined CoM].
#  The result is that the shadow-child state now matches closely with the
# real-child state, and they both follow 2m behind the parent..
exec(open("RUN_1_with_detach/input.py").read())
vehicle.shadow.traj.correct_for_offsets = True
