exec( open("RUN_R3/input.py").read())
# Calculate position and velocity from NULL params structure
test.run_compute_velocity_from_params = True
test.compute_with_null = True
test.pos_angle_in = trick.TargetRelative_StateParameter.INPUT_THETA_ROT
trick.stop(0)
