exec(open("RUN_att_SunPointing/input.py").read())

# Attitude input of SunPointing does not work with trans_init
# Set position input to TransInit to show incompatibility
vehicle.state_initialize.position_input_data_type = trick.StateInitialize.TransInit

vehicle.state_initialize.trans_init.reference_ref_frame_name = "Earth.inertial"
vehicle.state_initialize.trans_init.body_frame_id = "composite_body"
vehicle.state_initialize.trans_init.position = [6600000.0, 0.0, 0.0]
vehicle.state_initialize.velocity_input_data_type = trick.StateInitialize.Off
