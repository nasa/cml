exec(open("common_input.py").read())
log_control_flags()
log_hang_roll_angles()
log_topodetic_ref( with_rel_vec=True)

exec(open('Modified_data/set_hangroll_body_vectors.py').read())

vehicle.planet_rel_state.subscribe_hang_roll()
