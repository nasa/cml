drg = trick.DRAscii("test_data")
drg.set_cycle(1)
drg.freq = trick.DR_Always
trick.add_data_record_group(drg, trick.DR_Buffer)

drg.add_variable("test.in_rectangle_ang")
drg.add_variable("test.in_rectangle_cart")
drg.add_variable("test.in_hex_ang")
drg.add_variable("test.in_hex_cart")
