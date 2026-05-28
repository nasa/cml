exec(open("Modified_data/common_input.py").read())

# Call TR_param.compute_params_from_vectors before initialization
# This will produce an error message.
vehicle.state_initialize.TR_param.compute_params_from_vectors_SWIG([0,0,0], [0,0,0], vehicle.param_set)
