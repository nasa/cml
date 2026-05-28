drg = trick.DRAscii("test_data")
drg.set_cycle(1.0)
drg.freq = trick.DR_Always
trick.add_data_record_group(drg, trick.DR_Buffer)

def log_3vec( drg_, var):
  for ii in range(3):
    drg_.add_variable(var + "[%d]" %ii)

def log_3x3( drg_, var):
  for ii in range(3):
    log_3vec(drg_, var + "[%d]" %ii)

log_3vec( drg, "vehicle.body.composite_body.state.trans.position")
log_3vec( drg, "vehicle.body.composite_body.state.trans.velocity")
log_3x3(  drg, "vehicle.body.composite_body.state.rot.T_parent_this")
log_3vec( drg, "vehicle.body.composite_body.state.rot.ang_vel_this")
log_3vec( drg, "vehicle.body.composite_body.state.rot.Q_parent_this.vector")
drg.add_variable("vehicle.body.composite_body.state.rot.Q_parent_this.scalar")
