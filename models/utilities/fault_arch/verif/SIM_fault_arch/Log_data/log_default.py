dr_group = trick.DRAscii("fault_variables")
dr_group.set_cycle(1.0)
dr_group.set_freq(trick.DR_Always)
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable("test_object.constant_value")
dr_group.add_variable("test_object.constant_value_replaced")
dr_group.add_variable("test_object.linear_value")
dr_group.add_variable("test_object.counting_int")
