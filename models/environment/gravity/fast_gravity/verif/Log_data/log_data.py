def log_test_data ( log_cycle ) :
  dr_group = trick.DRAscii("test_data")
  dr_group.thisown = 0
  dr_group.set_cycle(log_cycle)
  dr_group.freq = trick.DR_Always

  dr_group.add_variable("vehicle.grav_controls_earth.gradient")
  dr_group.add_variable("vehicle.body.grav_interaction.grav_pot")
  for ii in range(0,3) :
    dr_group.add_variable("vehicle.body.composite_body.state.trans.position[" + str(ii) + "]" )
    dr_group.add_variable("vehicle.body.composite_body.state.trans.velocity[" + str(ii) + "]" )
    dr_group.add_variable("vehicle.body.grav_interaction.grav_accel[" + str(ii) + "]" )

  trick.add_data_record_group(dr_group, trick.DR_Buffer)

  return

def log_fast_grav( log_cycle) :
  dr_group = trick.DRAscii("fast_grav_data")
  dr_group.thisown = 0
  dr_group.set_cycle(log_cycle)
  dr_group.freq = trick.DR_Always
  dr_group.add_variable("vehicle.grav_controls_earth.count_limit")
  dr_group.add_variable("vehicle.grav_controls_earth.threshold_ratio_delta_acc")
  dr_group.add_variable("vehicle.grav_controls_earth.count")
  trick.add_data_record_group(dr_group, trick.DR_Buffer)
  return

