exec(open("Modified_data/common_input.py").read())

vehicle.state_initialize.attitude_input_data_type = trick.StateInitialize.MatrixInertialBody

# This is not a transformation matrix, so it will be modified to be one 
vehicle.state_initialize.T_inrtl_body = [[0.2, 0.5, 0.7],[-0.5,0.5,0.2],[0.1, 0.0, 0.9]]
