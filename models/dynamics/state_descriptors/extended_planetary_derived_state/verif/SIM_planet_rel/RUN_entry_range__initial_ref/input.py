exec(open("common_input.py").read())
log_control_flags()
log_entry_range()

vehicle.planet_rel_state.subscribe_entry_range()

# Position close to the initialization position set in env_setup.py
exec(open("Modified_data/set_entry_reference.py").read())
