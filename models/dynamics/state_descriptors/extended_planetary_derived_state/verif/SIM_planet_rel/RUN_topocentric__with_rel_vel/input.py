exec(open("common_input.py").read())
log_control_flags()
log_topocentric_ref( with_rel_vec=True)

vehicle.planet_rel_state.subscribe_topocentric()
vehicle.planet_rel_state.subscribe_rel_vel()
