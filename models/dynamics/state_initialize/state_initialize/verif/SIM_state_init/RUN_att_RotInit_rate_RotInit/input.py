exec(open("FAIL_att_RotInit/input.py").read())

vehicle.state_initialize.att_rate_input_data_type = trick.StateInitialize.RotInit

vehicle.state_initialize.rot_init.ang_velocity = [-1.0, 1.0, 2.0]
