exec(open("Modified_data/common_input.py").read())

vehicle.state_initialize.attitude_input_data_type = trick.StateInitialize.EulerNED_YPR

vehicle.state_initialize.ref_point_longitude = math.pi/2
vehicle.state_initialize.ref_point_latitude = 0.0

vehicle.state_initialize.yaw = 0.0
vehicle.state_initialize.pitch = 0.0
vehicle.state_initialize.roll = 0.0
