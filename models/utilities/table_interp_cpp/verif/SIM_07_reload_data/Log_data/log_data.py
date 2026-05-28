drg = trick.DRAscii("multi_data")
drg.set_cycle(1.0)
drg.freq = trick.DR_Always

drg.add_variable("multi.independent_variable")
drg.add_variable("multi.dependent_variable[0]")
drg.add_variable("multi.dependent_variable[1]")
drg.add_variable("multi.dependent_variable[2]")
drg.add_variable("multi.dependent_variable[3]")

trick.add_data_record_group(drg, trick.DR_Buffer)