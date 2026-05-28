dr_group = trick.sim_services.DRAscii("test_data")
dr_group.set_cycle(5000)
dr_group.freq = trick.sim_services.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable("vehicle.eclipse.illum_factor")
dr_group.add_variable("vehicle.eclipse.earth.illum_factor")
dr_group.add_variable("vehicle.eclipse.moon.illum_factor")
