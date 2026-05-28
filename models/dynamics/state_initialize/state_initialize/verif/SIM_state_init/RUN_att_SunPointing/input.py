exec(open("Modified_data/common_input.py").read())

vehicle.state_initialize.attitude_input_data_type = trick.StateInitialize.SunPointing_YPR

exec(open("Modified_data/set_initial_state.py").read())
