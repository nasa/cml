dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always

dr_group.add_variable("test.vehicle_position.ellip_coords.latitude" )
dr_group.add_variable("test.vehicle_position.ellip_coords.longitude" )
dr_group.add_variable("test.vehicle_position.ellip_coords.altitude" )

def log_range_model(model):
  dr_group.add_variable(f"test.{model}.totalrange_angle")
  dr_group.add_variable(f"test.{model}.crossrange_angle")
  dr_group.add_variable(f"test.{model}.downrange_angle")

  dr_group.add_variable(f"test.{model}.cross_range_avg_rad")
  dr_group.add_variable(f"test.{model}.down_range_avg_rad")
  dr_group.add_variable(f"test.{model}.total_range_avg_rad")

  dr_group.add_variable(f"test.{model}.cross_range_ref_rad")
  dr_group.add_variable(f"test.{model}.down_range_ref_rad")
  dr_group.add_variable(f"test.{model}.total_range_ref_rad")

trick.add_data_record_group(dr_group, trick.DR_Buffer)


def log_debug ( log_cycle ) :
  dr_group = trick.DRAscii("debug")
  dr_group.thisown = 0
  dr_group.set_cycle(log_cycle)
  dr_group.freq = trick.DR_Always

  for ii in range(3) :
      for jj in range(3) :
        dr_group.add_variable(f"test.earth.pfix.state.rot.T_parent_this[{ii}][{jj}]")

  for ii in range(3) :
      dr_group.add_variable(f"test.vehicle_position.cart_coords[{ii}]")

  for model in ["range_to_pfix", "range_to_pfix_internal_ref", "range_from_pfix"]:
    dr_group.add_variable(f"test.{model}.reference_data.position_type")
    dr_group.add_variable(f"test.{model}.reference_data.direction_type")
    for ii in range(3) :
        dr_group.add_variable(f"test.{model}.reference_data.position[{ii}]")
    for ii in range(3) :
        dr_group.add_variable(f"test.{model}.reference_data.direction[{ii}]")
    dr_group.add_variable(f"test.{model}.reference_data.azimuth")
    dr_group.add_variable(f"test.{model}.reference_data.latitude")
    dr_group.add_variable(f"test.{model}.reference_data.longitude")
    dr_group.add_variable(f"test.{model}.reference_data.altitude")

  trick.add_data_record_group(dr_group, trick.DR_Buffer)
