exec(open("common_input.py").read())
log_control_flags()
log_entry_range()

vehicle.planet_rel_state.subscribe_entry_range()

# Position matches the landing and launch ref points
exec(open("Modified_data/set_entry_to_launch.py").read())
