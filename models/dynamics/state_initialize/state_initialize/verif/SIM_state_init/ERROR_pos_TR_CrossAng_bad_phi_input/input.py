exec(open("Modified_data/common_input.py").read())

# Reference position vector:  North pole
# Target position vector:     On equator at 0 deg longituide
# Cross angle:                1.0 deg --> Position vector will be rotated along the great circle with 0 deg longitude
# Range angle:                0.9 deg --> Position vector will be rotated to a latitude of 30 deg
#
# The range and cross angle inputs result in a physically unrealizable position.
# This will generate an error message.

vehicle.state_initialize.position_input_data_type        = trick.StateInitialize.TR_GeodAlt_RngAng_CrossAng_AbsRotAng_LT_90
vehicle.state_initialize.ref_point_altitude              = 0.0
vehicle.state_initialize.ref_point_latitude              = trick.attach_units("degree", 90.0)
vehicle.state_initialize.ref_point_longitude             = 0.0

vehicle.state_initialize.TR_param.target_point_altitude  = 0.0
vehicle.state_initialize.TR_param.target_point_latitude  = 0.0
vehicle.state_initialize.TR_param.target_point_longitude = 0.0

vehicle.state_initialize.TR_geodetic_altitude            = trick.attach_units("ft", 400000.0)
vehicle.state_initialize.TR_theta_Rng                    = trick.attach_units("degree", 0.9)
vehicle.state_initialize.TR_phi_Cross                    = trick.attach_units("degree", 1.0)

vehicle.state_initialize.position = [0.0, 0.0, 0.0]
