exec(open("Modified_data/common_input.py").read())

vehicle.state_initialize.att_rate_input_data_type = trick.StateInitialize.RotInit

vehicle.state_initialize.rot_init.set_subject_body( vehicle.body )
vehicle.state_initialize.rot_init.reference_ref_frame_name = "Earth.inertial"
vehicle.state_initialize.rot_init.body_frame_id = "composite_body"
vehicle.state_initialize.rot_init.ang_velocity = [1.0, -1.0, 2.0]
