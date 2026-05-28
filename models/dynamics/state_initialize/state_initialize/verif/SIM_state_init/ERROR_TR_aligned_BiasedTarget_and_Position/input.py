exec(open("Modified_data/common_input.py").read())

# Reference position vector:  North pole
# Target position vector:     On equator at 0 deg longituide
# Rotation angle:              90 deg --> Range angle will be along equator
# Range angle:                  5 deg --> Position vector will be rotated 5 deg along the equator from the target
# Position should end up on equator at -5 deg longitude

vehicle.state_initialize.position_input_data_type        = trick.StateInitialize.TR_GeodAlt_RngAng_RotAng
vehicle.state_initialize.ref_point_altitude              = 0.0
vehicle.state_initialize.ref_point_latitude              = trick.attach_units("degree", 90.0)
vehicle.state_initialize.ref_point_longitude             = 0.0

vehicle.state_initialize.TR_param.target_point_altitude  = 0.0
vehicle.state_initialize.TR_param.target_point_latitude  = 0.0
vehicle.state_initialize.TR_param.target_point_longitude = 0.0

vehicle.state_initialize.TR_geodetic_altitude            = trick.attach_units("ft", 400000.0)
vehicle.state_initialize.TR_theta_Rng                    = trick.attach_units("degree", 5.0)
vehicle.state_initialize.TR_theta_Rot                    = trick.attach_units("degree", 90.0)

vehicle.state_initialize.velocity_input_data_type        = trick.StateInitialize.TR_InertialSpeedGammaLatang
vehicle.state_initialize.speed                           = 1.0
vehicle.state_initialize.flight_path_angle               = trick.attach_units("degree", 30.0)
vehicle.state_initialize.TR_Lambda                       = 0.0

vehicle.state_initialize.position = [0.0, 0.0, 0.0]
vehicle.state_initialize.velocity = [0.0, 0.0, 0.0]

# Add planet rotation
# With these settings, the biased target vector is aligned
# with the landing site position.  This will cause a divide-by-zero
# situation.  A pair of error messages will be posted.
vehicle.state_initialize.TR_param.K_theta   = 11.46870921
earth.planet.pfix.state.rot.ang_vel_this[2] = 1.0
