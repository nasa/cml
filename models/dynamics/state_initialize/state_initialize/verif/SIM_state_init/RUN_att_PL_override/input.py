exec(open("RUN_att_PL/input.py").read())

# Apply override body sequence
# Pitch angle becomes yaw angle
# Yaw angle becomes pitch angle
vehicle.state_initialize.ref_body_sequence_override = trick.Orientation.Yaw_Pitch_Roll
trick.CMLMessage.set_publish_level (trick.CMLMessage.Inform)
