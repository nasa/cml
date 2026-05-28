exec(open("common_input.py").read())
log_control_flags()
log_topocentric_ref()

vehicle.planet_rel_state.subscribe_topocentric()
