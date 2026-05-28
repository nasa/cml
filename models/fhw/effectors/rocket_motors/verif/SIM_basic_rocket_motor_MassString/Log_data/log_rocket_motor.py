dr_group = trick.sim_services.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.sim_services.DR_Always

dr_group.add_variable("rocket_motor.motor_prop_a.core_properties.mass")
dr_group.add_variable("rocket_motor.motor_prop_b.core_properties.mass")

trick.add_data_record_group(dr_group, trick.DR_Buffer)
