exec(open("Modified_data/common_input.py").read())

# Align the Target and Reference position vectors
# This will result in termination of the simulation.
vehicle.state_initialize.position_input_data_type        = trick.StateInitialize.TR_GeodAlt_RngAng_RotAng
vehicle.state_initialize.ref_point_altitude              = 0.0
vehicle.state_initialize.ref_point_latitude              = 0.0
vehicle.state_initialize.ref_point_longitude             = 0.0
vehicle.state_initialize.TR_param.target_point_altitude  = vehicle.state_initialize.ref_point_altitude
vehicle.state_initialize.TR_param.target_point_latitude  = vehicle.state_initialize.ref_point_latitude
vehicle.state_initialize.TR_param.target_point_longitude = vehicle.state_initialize.ref_point_longitude
