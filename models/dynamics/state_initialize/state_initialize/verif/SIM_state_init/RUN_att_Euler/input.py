exec(open("Modified_data/common_input.py").read())

# Redundant to re-set this, as it's already set in Modified_data/common_input.py
# Should get same attitude results as common_input
vehicle.state_initialize.attitude_input_data_type = trick.StateInitialize.EulerInertial_YPR
