exec(open("Modified_data/common_input.py").read())

# Call TR_param.set_planet with NULL planet pointer. This will trigger the
# fail() in CorrelatedStateDispersion::transform_TR_param.
vehicle.state_initialize.TR_param.set_planet(None)
