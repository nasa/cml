dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable("vehicle.radius")
dr_group.add_variable("vehicle.longitude")
dr_group.add_variable("vehicle.latitude")
dr_group.add_variable("vehicle.gravity_magnitude")
dr_group.add_variable("vehicle.gravity_radial_alignment")
