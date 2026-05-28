dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always

dr_group.add_variable("test.independent")
dr_group.add_variable("test.dependent")

trick.add_data_record_group(dr_group, trick.DR_Buffer)
