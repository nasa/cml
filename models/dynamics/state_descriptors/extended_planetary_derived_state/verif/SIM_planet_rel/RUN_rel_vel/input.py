exec(open("common_input.py").read())
log_control_flags()
log_relative_velocity()

vehicle.planet_rel_state.subscribe_rel_vel()
