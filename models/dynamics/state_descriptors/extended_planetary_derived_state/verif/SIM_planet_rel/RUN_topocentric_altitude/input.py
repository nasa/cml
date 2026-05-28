exec(open("common_input.py").read())
log_control_flags()
log_topocentric_ref(with_altitude = True)

vehicle.planet_rel_state.subscribe_topocentric_altitude()
