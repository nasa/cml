def log_test_data ( log_cycle ) :
  dr_group = trick.DRAscii("test_data")
  dr_group.thisown = 0
  dr_group.set_cycle(log_cycle)
  dr_group.freq = trick.DR_Always

  dr_group.add_variable("mass_test.tank.core_properties.mass")
  for ii in range(3) :
    dr_group.add_variable(
        "mass_test.tank.core_properties.position[%d]" %ii)
  for ii in range(3) :
    for jj in range (3) :
      dr_group.add_variable(
        "mass_test.tank.core_properties.inertia[%d][%d]" %(ii, jj))

  trick.add_data_record_group(dr_group, trick.DR_Buffer)
