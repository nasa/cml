exec(open("Modified_data/common_input.py").read())

vehicle.state_initialize.position_input_data_type = trick.StateInitialize.TransInit

vehicle.state_initialize.trans_init.reference_ref_frame_name = "Earth.inertial"
vehicle.state_initialize.trans_init.body_frame_id = "composite_body"
vehicle.state_initialize.trans_init.position = [6600000.0, 0.0, 0.0]
