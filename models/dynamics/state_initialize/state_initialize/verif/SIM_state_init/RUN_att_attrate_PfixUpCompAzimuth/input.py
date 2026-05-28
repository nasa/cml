exec(open("Modified_data/common_input.py").read())

vehicle.state_initialize.attitude_input_data_type = trick.StateInitialize.PfixUpCompAzimuth
vehicle.state_initialize.att_rate_input_data_type = trick.StateInitialize.PfixUpCompAzimuth

vehicle.state_initialize.ref_point_longitude = 1.0
vehicle.state_initialize.ref_point_latitude = 0.5
vehicle.state_initialize.azimuth = math.pi
