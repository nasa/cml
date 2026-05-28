def log_data_angle_interp1d(log_cycle):
  dr_grp = trick.DRAscii("test_data_angle_interp1d")
  dr_grp.thisown = 0
  dr_grp.set_cycle(log_cycle)
  dr_grp.freq = trick.DR_Always
  dr_grp.add_variable("test_angle_interp1d.var_x")
  dr_grp.add_variable("test_angle_interp1d.low_index")
  dr_grp.add_variable("test_angle_interp1d.frac")
  dr_grp.add_variable("test_angle_interp1d.interp_val")
  trick.add_data_record_group(dr_grp, trick.DR_Buffer) 
  return
