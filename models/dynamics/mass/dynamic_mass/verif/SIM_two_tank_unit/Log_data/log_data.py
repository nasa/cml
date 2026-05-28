def log_test_data ( log_cycle ) :
  dr_group = trick.DRAscii("test_data")
  dr_group.thisown = 0
  dr_group.set_cycle(log_cycle)
  dr_group.freq = trick.DR_Always

  dr_group.add_variable("mass_test.tank_a.core_properties.mass")
  dr_group.add_variable("mass_test.tank_b.core_properties.mass")
  dr_group.add_variable("mass_test.body.mass.composite_properties.mass")

  trick.add_data_record_group(dr_group, trick.DR_Buffer)


def log_countdown() :
  dr_group = trick.DRAscii("countdown_data")
  dr_group.thisown = 0
  dr_group.set_cycle(1.0)
  dr_group.freq = trick.sim_services.DR_Always

  dr_group.add_variable("mass_test.group.countdown_to_root_test")
  trick.add_data_record_group(dr_group, trick.DR_Buffer)
