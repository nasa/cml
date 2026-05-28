def log_test_data ( log_cycle ) :
  dr_group = trick.DRAscii("test_data")
  dr_group.thisown = 0
  dr_group.set_cycle(log_cycle)
  dr_group.freq = trick.DR_Always

  def log_add_3vec(var):
    for ii in range(3):
      dr_group.add_variable(var + "[%d]" %ii)

  log_add_3vec("vehicle.body.composite_body.state.trans.position")
  log_add_3vec("vehicle.body.structure.state.trans.position")
  log_add_3vec("vehicle.body.mass.composite_properties.position")
  for var in [".composite_properties.mass", ".core_properties.inertia[0][0]"]:
    for ii in range(2):
      dr_group.add_variable("vehicle.tanks[%d]" %ii + var)

  dr_group.add_variable("vehicle.tanks[0].nominal_properties.core_mass")
  dr_group.add_variable("vehicle.tanks[0].nominal_properties.position[0]")
  dr_group.add_variable("vehicle.body.mass.composite_properties.mass")

  trick.add_data_record_group(dr_group, trick.DR_Buffer)
