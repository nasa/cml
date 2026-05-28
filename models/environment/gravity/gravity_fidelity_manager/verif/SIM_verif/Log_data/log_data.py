dr_group = trick.sim_services.DRAscii("test_data")
dr_group.thisown = 0
dr_group.set_cycle(1.0)
dr_group.freq = trick.sim_services.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable("vehicle.body.composite_body.state.trans.position[0]")
dr_group.add_variable("vehicle.grav_controls_earth.degree")
dr_group.add_variable("vehicle.grav_controls_earth.order")
dr_group.add_variable("vehicle.grav_controls_earth.spherical")
