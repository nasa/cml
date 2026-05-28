def log_add_3vec(variable):
  for ii in range(3):
    dr_group.add_variable(variable + "[%d]" %ii)

def log_atmos_exec_out(atmos_exec):
  for var in ["density", "dynamic_viscosity", "pressure", "temperature", "wind_vmag"]:
    dr_group.add_variable(atmos_exec + ".out." + var)


dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

if (log_extended):
  log_add_3vec("vehicle.body.composite_body.state.trans.position")
  log_add_3vec("vehicle.body.composite_body.state.trans.velocity")
  for ii in range(3):
    for jj in range(3):
      dr_group.add_variable("vehicle.body.composite_body.state.rot.T_parent_this[%d][%d]" %(ii, jj))
else:
  dr_group.add_variable("vehicle.body.composite_body.state.trans.position[0]")

dr_group.add_variable("vehicle.body_state.topocentric_altitude")
log_atmos_exec_out("vehicle.atmos_exec")

for chute in ["dchute", "mchute", "schute"]:
  dr_group.add_variable("vehicle." + chute + ".planet_rel_state.topocentric_altitude")
  log_atmos_exec_out("vehicle." + chute + ".atmos_exec")
