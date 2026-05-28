exec(open("Modified_data/common_input.py").read())

vehicle.state_initialize.attitude_input_data_type = trick.StateInitialize.RotInit

vehicle.state_initialize.rot_init.set_subject_body( vehicle.body )
vehicle.state_initialize.rot_init.reference_ref_frame_name = "Earth.inertial"
vehicle.state_initialize.rot_init.body_frame_id = "composite_body"
vehicle.state_initialize.rot_init.orientation.data_source = trick.Orientation.InputEulerRotation
vehicle.state_initialize.rot_init.orientation.euler_sequence = trick.Orientation.Yaw_Pitch_Roll
vehicle.state_initialize.rot_init.orientation.euler_angles = [0.0, 0.0, 0.0]
