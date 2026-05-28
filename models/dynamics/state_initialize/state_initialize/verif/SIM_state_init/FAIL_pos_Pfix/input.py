exec(open("Modified_data/common_input.py").read())

# Attempts to initialize position using planet-fixed frame and velocity using inertial frame.
# This will fail because when one of them uses planet-fixed, the other one must as well.
vehicle.state_initialize.position_input_data_type = trick.StateInitialize.Pfix
