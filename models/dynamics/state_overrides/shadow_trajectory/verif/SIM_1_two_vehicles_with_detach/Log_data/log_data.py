dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable("vehicle.parent.core_body.state.trans.position[0]")
dr_group.add_variable("vehicle.parent.composite_body.state.trans.position[0]")
dr_group.add_variable("vehicle.child.composite_body.state.trans.position[0]")
dr_group.add_variable("vehicle.shadow.body.composite_body.state.trans.position[0]")

dr_group.add_variable("vehicle.child.composite_body.state.trans.velocity[0]")
dr_group.add_variable("vehicle.shadow.body.composite_body.state.trans.velocity[0]")

dr_group.add_variable("vehicle.child_wrt_parent.rel_state.trans.position[0]")
dr_group.add_variable("vehicle.shadow_wrt_parent.rel_state.trans.position[0]")
dr_group.add_variable("vehicle.shadow_wrt_child.rel_state.trans.position[0]")

dr_group.add_variable("vehicle.parent.mass.composite_properties.mass")

def add_y_axis_log( dr_group):
  dr_group.add_variable("vehicle.parent.composite_body.state.trans.position[1]")
  dr_group.add_variable("vehicle.child.composite_body.state.trans.position[1]")
  dr_group.add_variable("vehicle.shadow.body.composite_body.state.trans.position[1]")
