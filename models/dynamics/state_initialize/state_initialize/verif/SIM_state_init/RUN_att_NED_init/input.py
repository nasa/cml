exec(open("Modified_data/common_input.py").read())

vehicle.state_initialize.attitude_input_data_type = trick.StateInitialize.RotInit_NED

vehicle.state_initialize.ned_rot_init.ref_body_name = "test_vehicle"
vehicle.state_initialize.ned_rot_init.orientation.data_source = trick.Orientation.InputEulerRotation
vehicle.state_initialize.ned_rot_init.orientation.euler_sequence = trick.Orientation.Pitch_Yaw_Roll
vehicle.state_initialize.ned_rot_init.orientation.euler_angles = [0.0, 0.0, 0.0]
vehicle.state_initialize.ned_rot_init.altlatlong_type = trick.NorthEastDown.elliptical
