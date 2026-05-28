vehicle.state_initialize.position_input_data_type = trick.StateInitialize.Inertial
vehicle.state_initialize.velocity_input_data_type = trick.StateInitialize.Inertial
vehicle.state_initialize.attitude_input_data_type = trick.StateInitialize.EulerInertial_YPR
vehicle.state_initialize.att_rate_input_data_type = trick.StateInitialize.BodyInertialRate

import math
vehicle.state_initialize.yaw = math.pi/2
vehicle.state_initialize.pitch = 0.0
vehicle.state_initialize.roll = math.pi
vehicle.state_initialize.roll_rate_body = 1.0
vehicle.state_initialize.pitch_rate_body = -1.0
vehicle.state_initialize.yaw_rate_body = 2.0
vehicle.state_initialize.position = [6800000.0, 0.0, 0.0]
vehicle.state_initialize.velocity = [0.0, 8000.0, 0.0]
