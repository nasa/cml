dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable("msl_unit_test.geodetic_altitude")
dr_group.add_variable("msl_unit_test.geodetic_latitude")
dr_group.add_variable("msl_unit_test.longitude")
dr_group.add_variable("msl_unit_test.msl_alt.msl_altitude")