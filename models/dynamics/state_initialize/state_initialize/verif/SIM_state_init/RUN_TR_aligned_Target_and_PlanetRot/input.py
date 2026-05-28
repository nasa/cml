exec(open("Modified_data/common_input.py").read())

# Reference position vector:  Equator at the prime meridian
vehicle.state_initialize.position_input_data_type = trick.StateInitialize.TR_GeodAlt_RngAng_RotAng
vehicle.state_initialize.ref_point_altitude       = 0.0
vehicle.state_initialize.ref_point_latitude       = 0.0
vehicle.state_initialize.ref_point_longitude      = 0.0

# Align the Target and Planet Rotation vectors
# This will result in a warning.
vehicle.state_initialize.TR_param.target_point_altitude  = 0.0
vehicle.state_initialize.TR_param.target_point_latitude  = trick.attach_units("degree", 90.0)
vehicle.state_initialize.TR_param.target_point_longitude = 0.0

vehicle.state_initialize.TR_geodetic_altitude            = trick.attach_units("ft", 400000.0)
vehicle.state_initialize.TR_theta_Rng                    = trick.attach_units("degree",  90.0)
vehicle.state_initialize.TR_theta_Rot                    = trick.attach_units("degree", -90.0)

#Add planet rotation
earth.planet.pfix.state.rot.ang_vel_this[2] = 0.1
