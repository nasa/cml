dr_group = trick.sim_services.DRAscii("test_data")
dr_group.set_cycle(0.5)
dr_group.freq = trick.sim_services.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable("ts_so.ts.sway_parallel")
dr_group.add_variable("ts_so.ts.sway_normal")
dr_group.add_variable("ts_so.ts.twist_angle")

def log_3vec( var):
  for ii in range(3) :
    dr_group.add_variable(var + "[%d]"%ii )

def log_add_mags():
  dr_group.add_variable("ts_so.ts.fast_mag.parallel")
  dr_group.add_variable("ts_so.ts.fast_mag.normal")
  dr_group.add_variable("ts_so.ts.fast_mag.twist")

  dr_group.add_variable("ts_so.ts.slow_mag.parallel")
  dr_group.add_variable("ts_so.ts.slow_mag.normal")
  dr_group.add_variable("ts_so.ts.slow_mag.twist")

def log_add_enu():
  log_3vec("ts_so.ts.dp_enu")
  log_3vec("ts_so.ts.dv_enu")
  log_3vec("ts_so.ts.w_ts_wrt_enu_in_enu")
  dr_group.add_variable("ts_so.ts_y_in_enu[2]")
  dr_group.add_variable("ts_so.ts_z_in_enu[0]")
  dr_group.add_variable("ts_so.ts_x_in_enu[1]")

def log_add_ecef():
  log_3vec("ts_so.ts.dp_ecef")
  log_3vec("ts_so.ts.dv_ecef")
  log_3vec("ts_so.ts.w_ts_wrt_enu_in_ecef")
  # Note -- choice of which attitude variables to log depends on the
  # configuration so are assigned in the specific input files.

def log_all():
  log_add_mags()
  log_add_enu()
  log_add_ecef()

