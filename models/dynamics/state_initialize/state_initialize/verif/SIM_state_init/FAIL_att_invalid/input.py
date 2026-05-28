exec(open("Modified_data/common_input.py").read())

# Set attitude input to number that doesn't correlate to an option
vehicle.state_initialize.attitude_input_data_type = 100
