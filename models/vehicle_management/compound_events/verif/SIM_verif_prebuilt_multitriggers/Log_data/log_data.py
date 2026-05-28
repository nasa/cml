drg = trick.DRAscii("test_data")
drg.set_cycle(1)
drg.freq = trick.DR_Always
trick.add_data_record_group(drg, trick.DR_Buffer)

drg.add_variable("verif.sim_data.action_assgt")
