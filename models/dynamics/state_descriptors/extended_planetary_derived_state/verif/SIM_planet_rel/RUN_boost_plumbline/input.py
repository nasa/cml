exec(open("common_input.py").read())
log_control_flags()
log_boost_ref()
log_plumbline_ref()

vehicle.planet_rel_state.subscribe_boost_reference()
vehicle.planet_rel_state.subscribe_plumbline()
exec(open("Modified_data/set_plumbline_boost_reference.py").read())
