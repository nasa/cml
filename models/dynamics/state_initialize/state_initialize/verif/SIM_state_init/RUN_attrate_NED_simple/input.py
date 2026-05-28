exec(open("Modified_data/common_input.py").read())

vehicle.state_initialize.att_rate_input_data_type = trick.StateInitialize.EulerNED_YPR

vehicle.state_initialize.roll_rate_body = 2.0
vehicle.state_initialize.pitch_rate_body = 1.0
vehicle.state_initialize.yaw_rate_body = -1.0

