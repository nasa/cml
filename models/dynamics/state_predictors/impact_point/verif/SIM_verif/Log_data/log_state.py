def log_add_3vec( drg, var):
  for ii in range (3):
    drg.add_variable( var + "[%d]" %ii)

dr_group = trick.DRAscii("verif")
dr_group.set_cycle(5.0)
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable("impact_point.impact_point.time_to_impact" )
dr_group.add_variable("impact_point.impact_point.time_of_impact" )
dr_group.add_variable("impact_point.impact_point.surface_range" )
dr_group.add_variable("impact_point.impact_point.impact_type" )

dr_group.add_variable("impact_point.impact_point.ellip_coords.longitude" )
dr_group.add_variable("impact_point.impact_point.ellip_coords.latitude" )
dr_group.add_variable("impact_point.impact_point.ellip_coords.altitude" )

def log_extended(log_cycle):
  dr_group2 = trick.DRAscii("extended")
  dr_group2.set_cycle(log_cycle)
  dr_group2.thisown = 0
  dr_group2.freq = trick.DR_Always
  trick.add_data_record_group(dr_group2, trick.DR_Buffer)

  log_add_3vec( dr_group2, "vehicle.dyn_body.structure.state.trans.position")
  log_add_3vec( dr_group2, "vehicle.dyn_body.structure.state.trans.velocity")
  log_add_3vec( dr_group2, "impact_point.impact_point.cart_coords")
  log_add_3vec( dr_group2, "impact_point.planet_rel_state.state.cart_coords")

  dr_group2.add_variable("impact_point.impact_point.sphere_coords.longitude" )
  dr_group2.add_variable("impact_point.impact_point.sphere_coords.latitude" )
  dr_group2.add_variable("impact_point.impact_point.sphere_coords.altitude" )

  dr_group2.add_variable("impact_point.planet_rel_state.state.ellip_coords.latitude")
  dr_group2.add_variable("impact_point.planet_rel_state.state.ellip_coords.altitude")
  dr_group2.add_variable("impact_point.planet_rel_state.state.ellip_coords.longitude")
