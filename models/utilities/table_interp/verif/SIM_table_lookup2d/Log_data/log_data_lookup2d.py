def log_data_lookup2d(log_cycle):
  dr_grp = trick.DRAscii("test_data_lookup2d")
  dr_grp.thisown = 0
  dr_grp.set_cycle(log_cycle)
  dr_grp.freq = trick.DR_Always
  dr_grp.add_variable("test_lookup2d.var_x")
  dr_grp.add_variable("test_lookup2d.var_y")
  dr_grp.add_variable("test_lookup2d.interp_val")
  trick.add_data_record_group(dr_grp, trick.DR_Buffer) 
  return

