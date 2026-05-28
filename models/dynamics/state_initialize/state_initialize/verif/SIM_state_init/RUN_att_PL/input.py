exec(open("Modified_data/common_input.py").read())

vehicle.state_initialize.attitude_input_data_type = trick.StateInitialize.EulerPlumbline_PYR

vehicle.state_initialize.rot_ref_longitude = math.pi/2
vehicle.state_initialize.rot_ref_latitude = 0.0
vehicle.state_initialize.rot_ref_azimuth = -math.pi/2
