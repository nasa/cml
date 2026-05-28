def log_3_vec(drg_, var):
  for ii in range(3) :
    drg_.add_variable( var + "[%d]"  %ii)

drg = trick.sim_services.DRAscii("test_data")
drg.set_cycle(1.0)
drg.freq = trick.sim_services.DR_Always
trick.add_data_record_group(drg, trick.DR_Buffer)


drg.add_variable( "test.lagged_altitude")
drg.add_variable( "test.lagged_data.data_out.density")
drg.add_variable( "test.lagged_data.data_out.speed_of_sound")
log_3_vec( drg, "test.lagged_data.data_out.planetodetic_wind_velocity")
