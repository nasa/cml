exec(open("common_input.py").read())
log_control_flags()
log_entry_range()

vehicle.planet_rel_state.subscribe_entry_range()

# Position set mid-sim
trick.add_read(20.0, """
vehicle.planet_rel_state.entry_range.set_reference_data_from_inrtl_state(
                                      vehicle.body.composite_body.state.trans)
""");
