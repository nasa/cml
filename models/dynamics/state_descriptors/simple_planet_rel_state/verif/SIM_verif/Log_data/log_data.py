def log_3vec( drg, var):
  for ii in range(3):
    drg.add_variable( var + "[" + str(ii) + "]")

def log_3x3( drg, var):
  for jj in range (3):
    log_3vec( drg, var + "[" + str(jj) + "]")

dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

log_3vec( dr_group, "vehicle.body.composite_body.state.trans.position")
log_3vec( dr_group, "vehicle.planet_rel.rel_state.trans.position")
dr_group.add_variable("vehicle.planet_rel.altitude")
log_3vec( dr_group, "vehicle.body.composite_body.state.trans.velocity")
log_3vec( dr_group, "vehicle.planet_rel.rel_state.trans.velocity")
log_3vec( dr_group, "vehicle.body.composite_body.state.rot.ang_vel_this")
log_3vec( dr_group, "vehicle.planet_rel.rel_state.rot.ang_vel_this")

log_3x3( dr_group, "vehicle.body.composite_body.state.rot.T_parent_this")
log_3x3( dr_group, "vehicle.planet_rel.rel_state.rot.T_parent_this")
