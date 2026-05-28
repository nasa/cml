exec(open("common_input.py").read())
log_control_flags()
log_landing_range()

vehicle.planet_rel_state.subscribe_landing_range()
exec(open("Modified_data/set_landing_reference.py").read())
