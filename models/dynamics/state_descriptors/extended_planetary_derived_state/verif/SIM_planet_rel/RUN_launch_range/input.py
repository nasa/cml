exec(open("common_input.py").read())
log_control_flags()
log_launch_range()
log_entry_range()

vehicle.planet_rel_state.subscribe_launch_range()
vehicle.planet_rel_state.subscribe_entry_range()
exec(open("Modified_data/set_launch_reference.py").read())
