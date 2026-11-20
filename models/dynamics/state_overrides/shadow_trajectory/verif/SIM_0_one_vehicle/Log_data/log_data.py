dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable("vehicle.body.composite_body.state.trans.position[0]")
dr_group.add_variable("vehicle.shadow.body.composite_body.state.trans.position[0]")
dr_group.add_variable("vehicle.body.composite_body.state.trans.velocity[0]")
dr_group.add_variable("vehicle.shadow.body.composite_body.state.trans.velocity[0]")

dr_group.add_variable("vehicle.body.composite_body.state.rot.Q_parent_this.vector[0]")
dr_group.add_variable("vehicle.shadow.body.composite_body.state.rot.Q_parent_this.vector[0]")

dr_group.add_variable("vehicle.body.composite_body.state.rot.ang_vel_this[0]")
dr_group.add_variable("vehicle.shadow.body.composite_body.state.rot.ang_vel_this[0]")
