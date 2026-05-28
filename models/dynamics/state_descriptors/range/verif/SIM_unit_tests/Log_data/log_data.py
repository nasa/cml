dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always

dr_group.add_variable("test.vehicle_position.ellip_coords.latitude" )
dr_group.add_variable("test.vehicle_position.ellip_coords.longitude" )
dr_group.add_variable("test.vehicle_position.ellip_coords.altitude" )

def log_range_model(model):
  dr_group.add_variable("test.%s.totalrange_angle" %model)
  dr_group.add_variable("test.%s.crossrange_angle" %model)
  dr_group.add_variable("test.%s.downrange_angle" %model)

  dr_group.add_variable("test.%s.cross_range_avg_rad" %model)
  dr_group.add_variable("test.%s.down_range_avg_rad" %model)
  dr_group.add_variable("test.%s.total_range_avg_rad" %model)

  dr_group.add_variable("test.%s.cross_range_ref_rad" %model)
  dr_group.add_variable("test.%s.down_range_ref_rad" %model)
  dr_group.add_variable("test.%s.total_range_ref_rad" %model)

trick.add_data_record_group(dr_group, trick.DR_Buffer)


def log_debug ( log_cycle ) :
  dr_group = trick.DRAscii("debug")
  dr_group.thisown = 0
  dr_group.set_cycle(log_cycle)
  dr_group.freq = trick.DR_Always

  for ii in range(3) :
      for jj in range(3) :
        dr_group.add_variable("test.earth.pfix.state.rot.T_parent_this[%d][%d]" %(ii, jj))

  for ii in range(3) :
      dr_group.add_variable("test.vehicle_position.cart_coords[%d]" %ii)

  for model in ["range_to_pfix", "range_to_pfix_internal_ref", "range_from_pfix"]:
    dr_group.add_variable("test.%s.reference_data.position_type" %model)
    dr_group.add_variable("test.%s.reference_data.direction_type" %model)
    for ii in range(3) :
        dr_group.add_variable("test.%s.reference_data.position[%d]" %(model, ii))
    for ii in range(3) :
        dr_group.add_variable("test.%s.reference_data.direction[%d]" %(model, ii))
    dr_group.add_variable("test.%s.reference_data.azimuth" %model)
    dr_group.add_variable("test.%s.reference_data.latitude" %model)
    dr_group.add_variable("test.%s.reference_data.longitude" %model)
    dr_group.add_variable("test.%s.reference_data.altitude" %model)

  trick.add_data_record_group(dr_group, trick.DR_Buffer)
