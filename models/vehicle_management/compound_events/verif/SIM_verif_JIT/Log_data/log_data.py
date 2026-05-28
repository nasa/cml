drg = trick.DRAscii("test_data")
drg.set_cycle(1)
drg.freq = trick.DR_Always
trick.add_data_record_group(drg, trick.DR_Buffer)

drg.add_variable("verif.action_assgt")
drg.add_variable("verif.manager.events_ptr[0].event_triggered")

