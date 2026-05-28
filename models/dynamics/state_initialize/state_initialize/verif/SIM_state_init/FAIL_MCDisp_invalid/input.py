exec(open("RUN_MCDisp_inrtl/input.py").read())

# Set frame to invalid option (options correspond to 0, 1, 2)
vehicle.state_initialize.monte_carlo_dispersion.frame = 4
