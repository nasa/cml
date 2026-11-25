exec(open("FAIL_pos_TransInit/input.py").read())

vehicle.state_initialize.velocity_input_data_type = trick.cml.StateInitialize.TransInit
vehicle.state_initialize.trans_init.velocity = [0, 7800.0, 0.0]
