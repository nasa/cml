dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable("mslUnitTest.geodetic_altitude")
dr_group.add_variable("mslUnitTest.geodetic_latitude")
dr_group.add_variable("mslUnitTest.longitude")
dr_group.add_variable("mslUnitTest.msl_alt.mslAltitude")
