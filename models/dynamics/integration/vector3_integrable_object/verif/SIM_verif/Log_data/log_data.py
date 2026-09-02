def log_add_3vec(drg, var):
  for ii in range(3):
    drg.add_variable( f"{var}[{ii}]")

dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)
