exec(open("common_input.py").read())
log_control_flags()
log_range_safety()
log_launch_range()

vehicle.planet_rel_state.range_safety.XVRT_Azi = 3.14159265358979
vehicle.planet_rel_state.range_safety.YVRT_Azi = 1.57079632679490
vehicle.planet_rel_state.subscribe_range_safety()
vehicle.planet_rel_state.subscribe_launch_range()
exec(open("Modified_data/set_range_safety_reference.py").read())
