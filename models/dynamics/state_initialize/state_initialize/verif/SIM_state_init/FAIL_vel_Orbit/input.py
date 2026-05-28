exec(open("Modified_data/common_input.py").read())

vehicle.state_initialize.velocity_input_data_type = trick.StateInitialize.OrbitalElements

vehicle.state_initialize.orb_elem_init.set = trick.DynBodyInitOrbit.SmaEccIncAscnodeArgperTimeperi
vehicle.state_initialize.orb_elem_init.semi_major_axis = 6600000
vehicle.state_initialize.orb_elem_init.eccentricity    = 0.001
vehicle.state_initialize.orb_elem_init.inclination     = 0.0
vehicle.state_initialize.orb_elem_init.ascending_node  = 0.0
vehicle.state_initialize.orb_elem_init.arg_periapsis   = 0.0
vehicle.state_initialize.orb_elem_init.time_periapsis  = 0.0
