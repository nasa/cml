exec(open("Modified_data/common_input.py").read())

vehicle.state_initialize.att_rate_input_data_type = trick.StateInitialize.Random
vehicle.state_initialize.att_rate_random_seed = 2
vehicle.state_initialize.max_rate_random = 10
