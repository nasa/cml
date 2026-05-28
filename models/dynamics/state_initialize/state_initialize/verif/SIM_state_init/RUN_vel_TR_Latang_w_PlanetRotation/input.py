exec(open("Modified_data/common_input.py").read())

# Reference position vector:          Not used for target relative velocity definition, only for target relative position definition
# Position vector:                    On equator at  0 deg longitude
# Target position vector:             On equator at 45 deg longitude
# Inertial speed:                     1 m/s
# Inertial topocentric flight path:  30 deg
# Lateral angle:                     30 deg
# Velocity should end up with a 30 deg flight path angle and a 45 deg azimuth angle
# Note: This is the same result as without planet rotation because the position and target position are both on the equator.
#       Thus, when the target is rotated with the planet, it remains in the same plane relative to the position.

vehicle.state_initialize.position_input_data_type = trick.StateInitialize.NED_Geodetic
vehicle.state_initialize.ref_point_altitude  = 0.0
vehicle.state_initialize.ref_point_latitude  = 0.0
vehicle.state_initialize.ref_point_longitude = 0.0
vehicle.state_initialize.position = [0.0, 0.0, 0.0]

vehicle.state_initialize.velocity_input_data_type        = trick.StateInitialize.TR_InertialSpeedGammaLatang
vehicle.state_initialize.TR_param.target_point_altitude  = 0.0
vehicle.state_initialize.TR_param.target_point_latitude  = 0.0
vehicle.state_initialize.TR_param.target_point_longitude = trick.attach_units("degree", 45.0)

vehicle.state_initialize.speed                           = 1.0
vehicle.state_initialize.flight_path_angle               = trick.attach_units("degree", 30.0)
vehicle.state_initialize.TR_Lambda                       = trick.attach_units("degree", 30.0)

#Add planet rotation
vehicle.state_initialize.TR_param.K_theta   = 10
earth.planet.pfix.state.rot.ang_vel_this[2] = 0.1
