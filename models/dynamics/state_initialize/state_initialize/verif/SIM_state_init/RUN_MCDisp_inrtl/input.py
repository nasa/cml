# Test using orbital parameters for the initialization
exec(open("RUN_pos_Orbit_vel_Orbit/input.py").read())

vehicle.state_initialize.monte_carlo_dispersion.disperse_pv = True
vehicle.state_initialize.monte_carlo_dispersion.position_dispersion = [ 1, 2, 3]
vehicle.state_initialize.monte_carlo_dispersion.velocity_dispersion = [-1,-2,-3]
