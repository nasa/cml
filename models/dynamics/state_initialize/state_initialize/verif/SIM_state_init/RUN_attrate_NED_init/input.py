exec(open("Modified_data/common_input.py").read())

vehicle.state_initialize.att_rate_input_data_type = trick.StateInitialize.RotInit_NED

vehicle.state_initialize.ned_rot_init.ref_body_name = "test_vehicle"
vehicle.state_initialize.ned_rot_init.altlatlong_type = trick.NorthEastDown.elliptical
vehicle.state_initialize.ned_rot_init.ang_velocity = [1.0, -1.0, 2.0]
