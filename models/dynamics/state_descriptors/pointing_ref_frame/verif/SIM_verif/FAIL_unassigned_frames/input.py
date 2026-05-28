# RUN_10_unassigned_frames_FAIL
# With this run, the setting of the target and originating frames is disabled
# When the pointing-ref-frame goes through activation, it will drop an error
# message and will not be made available.
# When the relative-derived-state tries to configure, it will not be able to
# find the pointing ref-frame, which will shut down the simulation.

print("\n\n***************************************************************")
print(" Target and originating frame pointers unset")
print("***************************************************************")

frame.set_frames = False
exec(open("RUN_01_Geometric/input.py").read())
trick.stop(0)
