def log_data_interp4d(log_cycle):
  dr_grp = trick.DRAscii("test_data_interp4d")
  dr_grp.thisown = 0
  dr_grp.set_cycle(log_cycle)
  dr_grp.freq = trick.DR_Always
  dr_grp.add_variable("test_interp4d.var_x")
  dr_grp.add_variable("test_interp4d.low_index_x")
  dr_grp.add_variable("test_interp4d.frac_x")
  dr_grp.add_variable("test_interp4d.var_y")
  dr_grp.add_variable("test_interp4d.low_index_y")
  dr_grp.add_variable("test_interp4d.frac_y")
  dr_grp.add_variable("test_interp4d.var_z")
  dr_grp.add_variable("test_interp4d.low_index_z")
  dr_grp.add_variable("test_interp4d.frac_z")
  dr_grp.add_variable("test_interp4d.var_w")
  dr_grp.add_variable("test_interp4d.low_index_w")
  dr_grp.add_variable("test_interp4d.frac_w")

  dr_grp.add_variable("test_interp4d.interp_val")
  
  trick.add_data_record_group(dr_grp, trick.DR_Buffer) 
  return
