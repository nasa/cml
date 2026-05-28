exec(open("Modified_data/common_input.py").read())

# Reference position vector:          Not used for target relative velocity definition, only for target relative position definition
# Position vector:                    On equator at  0 deg longitude (see "Modified_data/nominal_state_init.py")
# Target position vector:             On equator at 45 deg longitude
# Inertial speed:                     1 m/s
# Inertial topocentric flight path:  30 deg
# Lateral angle:                     50 deg
# The lateral angle input is not physically realizable.
# This input will result in simulation termination.

vehicle.state_initialize.velocity_input_data_type        = trick.StateInitialize.TR_InertialSpeedGammaLatang

vehicle.state_initialize.TR_param.target_point_altitude  = 0.0
vehicle.state_initialize.TR_param.target_point_latitude  = 0.0
vehicle.state_initialize.TR_param.target_point_longitude = trick.attach_units("degree", 45.0)

vehicle.state_initialize.speed                           = 1.0
vehicle.state_initialize.flight_path_angle               = trick.attach_units("degree", 30.0)
vehicle.state_initialize.TR_Lambda                       = trick.attach_units("degree", 50.0)

