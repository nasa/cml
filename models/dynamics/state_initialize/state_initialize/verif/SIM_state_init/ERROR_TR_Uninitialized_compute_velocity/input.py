exec(open("Modified_data/common_input.py").read())

# Call TR_param.compute_velocity_from_params before intialization
# This will produce an error message.
vehicle.state_initialize.TR_param.compute_velocity_from_params_SWIG(0, 0, 0, [0,0,0], [0,0,0])
