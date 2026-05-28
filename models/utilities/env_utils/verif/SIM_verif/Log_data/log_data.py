dr_group = trick.sim_services.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.sim_services.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)


dr_group.add_variable("test.test1")
dr_group.add_variable("test.test2")
dr_group.add_variable("test.test3")
dr_group.add_variable("test.test4")
