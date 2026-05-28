dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(10.0)
dr_group.freq = trick.DR_Always

for ii in range(3) :
    dr_group.add_variable("vehicle.body.composite_body.state.trans.position[%d]" %ii)
for ii in range(3) :
    dr_group.add_variable("vehicle.body.composite_body.state.trans.velocity[%d]" %ii)

for ii in range(3) :
    for jj in range (3) :
      dr_group.add_variable("vehicle.body.composite_body.state.rot.T_parent_this[%d][%d]" %(ii, jj))
for ii in range(3) :
    for jj in range(3) :
      dr_group.add_variable("earth.planet.pfix.state.rot.T_parent_this[%d][%d]" %(ii, jj))

for ii in range(3) :
    dr_group.add_variable("vehicle.body.composite_body.state.rot.ang_vel_this[%d]" %ii)
for ii in range(3) :
    dr_group.add_variable("vehicle.pfix_position.cart_coords[%d]" %ii)

dr_group.add_variable("vehicle.pfix_position.sphere_coords.latitude" )
dr_group.add_variable("vehicle.pfix_position.sphere_coords.longitude" )
dr_group.add_variable("vehicle.pfix_position.sphere_coords.altitude" )

dr_group.add_variable("vehicle.range_to_pfix.totalrange_angle")
dr_group.add_variable("vehicle.range_to_pfix.crossrange_angle")
dr_group.add_variable("vehicle.range_to_pfix.downrange_angle")

dr_group.add_variable("vehicle.range_to_pfix.cross_range_avg_rad")
dr_group.add_variable("vehicle.range_to_pfix.down_range_avg_rad")
dr_group.add_variable("vehicle.range_to_pfix.total_range_avg_rad")

dr_group.add_variable("vehicle.range_to_pfix.cross_range_ref_rad")
dr_group.add_variable("vehicle.range_to_pfix.down_range_ref_rad")
dr_group.add_variable("vehicle.range_to_pfix.total_range_ref_rad")

dr_group.add_variable("vehicle.range_from_pfix.totalrange_angle")
dr_group.add_variable("vehicle.range_from_pfix.crossrange_angle")
dr_group.add_variable("vehicle.range_from_pfix.downrange_angle")

dr_group.add_variable("vehicle.range_from_pfix.cross_range_avg_rad")
dr_group.add_variable("vehicle.range_from_pfix.down_range_avg_rad")
dr_group.add_variable("vehicle.range_from_pfix.total_range_avg_rad")

dr_group.add_variable("vehicle.range_from_pfix.cross_range_ref_rad")
dr_group.add_variable("vehicle.range_from_pfix.down_range_ref_rad")
dr_group.add_variable("vehicle.range_from_pfix.total_range_ref_rad")


trick.add_data_record_group(dr_group, trick.DR_Buffer)
