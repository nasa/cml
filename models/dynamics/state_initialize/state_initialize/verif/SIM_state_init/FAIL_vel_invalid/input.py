exec(open("Modified_data/common_input.py").read())

# Set velocity input to a number that doesn't correlate to an option
vehicle.state_initialize.velocity_input_data_type = 100
