exec(open("common_input.py").read())
log_control_flags()
log_hang_roll_angles()

# This subscription should fail since inputs not set
vehicle.planet_rel_state.subscribe_hang_roll()

# Don't need to run that long, it's an init warning we want
# to see as well as all models being off
trick.sim_services.exec_set_terminate_time(10)

# Test that the subscriptions are off
assert vehicle.planet_rel_state.is_rel_vel_subscribed() == 0
assert vehicle.planet_rel_state.is_topocentric_subscribed() == 0
assert vehicle.planet_rel_state.is_topocentric_altitude_subscribed() == 0
assert vehicle.planet_rel_state.is_topodetic_subscribed() == 0
assert vehicle.planet_rel_state.is_hang_roll_subscribed() == 0
assert vehicle.planet_rel_state.is_boost_reference_subscribed() == 0
assert vehicle.planet_rel_state.is_plumbline_subscribed() == 0
assert vehicle.planet_rel_state.is_launch_range_subscribed() == 0
assert vehicle.planet_rel_state.is_landing_range_subscribed() == 0
assert vehicle.planet_rel_state.is_entry_range_subscribed() == 0
assert vehicle.planet_rel_state.is_range_safety_subscribed() == 0
assert vehicle.planet_rel_state.is_pt_to_pt_subscribed() == 0
