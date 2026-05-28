exec(open("common_input.py").read())
common_input(1.0)

# Use OrbitalElements for position to set use_trans_init False
# to elicit a warning
# Also sets use_orb_init to True to test that branch
test.state_init.position_input_data_type = trick.StateInitialize.OrbitalElements
test.state_init.orb_elem_init.set = trick.DynBodyInitOrbit.SmaEccIncAscnodeArgperTimeperi
test.state_init.orb_elem_init.semi_major_axis = 6600000
test.state_init.orb_elem_init.eccentricity    = 0.001
test.state_init.orb_elem_init.inclination     = 0.0
test.state_init.orb_elem_init.ascending_node  = 0.0
test.state_init.orb_elem_init.arg_periapsis   = 0.0
test.state_init.orb_elem_init.time_periapsis  = 0.0
test.state_init.velocity_input_data_type = trick.StateInitialize.Off
