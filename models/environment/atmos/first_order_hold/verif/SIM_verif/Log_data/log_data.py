def log_test_data ( log_cycle ) :
  recording_group_name =  "test_data"
  dr_group = trick.DRAscii(recording_group_name)
  dr_group.thisown = 0
  dr_group.set_cycle(log_cycle)
  dr_group.freq = trick.DR_Always
  dr_group.add_variable(   "test_object.test_wind")
  dr_group.add_variable(   "test_object.atmos_out.wind_velocity_td[0]")
  dr_group.add_variable(   "test_object.atmos_out.atmos_new_time")
  dr_group.add_variable(   "test_object.hold.reference_time")
  dr_group.add_variable(   "test_object.hold.slope_valid_time")
  dr_group.add_variable(   "test_object.hold.reference_wind[0]")
  dr_group.add_variable(   "test_object.hold.slope_reference_wind[0]")
  dr_group.add_variable(   "test_object.hold.slope[0]")
  dr_group.add_variable(   "test_object.atmos_out.wind_angle_blowing_from")
  dr_group.add_variable(   "test_object.atmos_out.wind_angle_blowing_to")


  trick.add_data_record_group(dr_group, trick.DR_Buffer)

  return
