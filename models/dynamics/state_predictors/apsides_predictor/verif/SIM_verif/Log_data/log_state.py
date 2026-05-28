def log_state( log_cycle ) :
  dr_group = trick.DRAscii("state")
  dr_group.thisown = 0
  dr_group.set_cycle(log_cycle)
  dr_group.freq = trick.DR_Always

  for ii in range(3) :
    dr_group.add_variable("vehicle.dyn_body.structure.state.trans.position[%d]" %ii)
  for ii in range(3) :
    dr_group.add_variable("vehicle.dyn_body.structure.state.trans.velocity[%d]" %ii)
  dr_group.add_variable("apsides_predictor.apsides.time_to_next_apsis" )
  dr_group.add_variable("apsides_predictor.apsides.next_apsis_type" )
  dr_group.add_variable("apsides_predictor.apsides.apoapsis_altitude" )
  dr_group.add_variable("apsides_predictor.apsides.periapsis_altitude" )

  dr_group.add_variable("apsides_predictor.planet_rel_state.state.sphere_coords.altitude")

  trick.add_data_record_group(dr_group, trick.DR_Buffer)

