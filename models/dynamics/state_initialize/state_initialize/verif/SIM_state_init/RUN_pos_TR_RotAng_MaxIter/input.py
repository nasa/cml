exec(open("Modified_data/common_input.py").read())

# Reference position vector:  North pole
# Target position vector:     On equator at 0 deg longituide
# Rotation angle:             -45 deg --> Range angle will be off equator
# Range angle:                 90 deg --> Position vector will be rotated 90 deg from the target vector
#
# From an off-line Matlab script, the position should be:
#   position = [+3.973573749838499e-10   +4.588655187784611e+06   +4.588655187784612e+06]

vehicle.state_initialize.position_input_data_type = trick.StateInitialize.TR_GeodAlt_RngAng_RotAng
vehicle.state_initialize.ref_point_altitude              = 0.0
vehicle.state_initialize.ref_point_latitude              = trick.attach_units("degree", 90.0)
vehicle.state_initialize.ref_point_longitude             = 0.0

vehicle.state_initialize.TR_param.target_point_altitude  = 0.0
vehicle.state_initialize.TR_param.target_point_latitude  = 0.0
vehicle.state_initialize.TR_param.target_point_longitude = 0.0

vehicle.state_initialize.TR_geodetic_altitude            = trick.attach_units("ft", 400000.0)
vehicle.state_initialize.TR_theta_Rng                    = trick.attach_units("degree",  90.0)
vehicle.state_initialize.TR_theta_Rot                    = trick.attach_units("degree", -45.0)

vehicle.state_initialize.position = [0.0, 0.0, 0.0]

# Set tolerance for position magnitude computation very small 
# so that max iteration limit is exceeded.  
# This will result in a warning.
vehicle.state_initialize.TR_param.tolerance  = 1e-16
