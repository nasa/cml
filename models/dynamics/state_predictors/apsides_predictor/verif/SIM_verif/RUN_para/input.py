exec(open("RUN_high_ecc/input.py").read())

# By default, OrbElemSubset considers a trajectory to be parabolic if the
# absolute value of its specific energy is at most 10^-6 J/kg. This requires
# an extremely precise velocity.
vehicle.trans_init.velocity = [0.0, 10827.5308344326, 0.0]
# Use a spherical gravity model; otherwise the trajectory won't remain parabolic
vehicle.grav_control.spherical = True
