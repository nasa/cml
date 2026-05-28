exec(open("Modified_data/common_input.py").read())

vehicle.state_initialize.velocity_input_data_type = trick.StateInitialize.PfixSpeedGammaAzimuth

vehicle.state_initialize.speed = 1
vehicle.state_initialize.azimuth = 0.523598775598
vehicle.state_initialize.flight_path_angle = 0.523598775598

# Set the earth rotating, otherwise this is identical to inertial case
earth.planet.pfix.state.rot.ang_vel_this = [0.0, 0.0, 7.2921151467063882e-05]
