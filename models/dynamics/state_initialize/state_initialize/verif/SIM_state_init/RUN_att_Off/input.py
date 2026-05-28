exec(open("Modified_data/common_input.py").read())

vehicle.state_initialize.attitude_input_data_type = trick.StateInitialize.Off

# Call free stream overwriting method to elicit warning
# since free stream is not necessary
vehicle.state_initialize.overwrite_attitude_from_free_stream([0,0,0])
