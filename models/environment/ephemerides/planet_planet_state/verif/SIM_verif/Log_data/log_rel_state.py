dr_group = trick.DRAscii("planet_rel_state")
dr_group.set_cycle(10.0)
dr_group.freq = trick.DR_Always


dr_group.add_variable("planet_rel_state.relstate_name.position_vector_only" )
dr_group.add_variable("planet_rel_state.relstate_name.state.rot.ang_vel_mag")
dr_group.add_variable("planet_rel_state.relstate_direct.position_vector_only" )
dr_group.add_variable("planet_rel_state.relstate_direct.state.rot.ang_vel_mag")

dr_group.add_variable("planet_rel_state.position_mag_name")
dr_group.add_variable("planet_rel_state.position_mag_direct")
dr_group.add_variable("planet_rel_state.velocity_mag_name")
dr_group.add_variable("planet_rel_state.velocity_mag_direct")

def log_add_3vec(variable):
  for ii in range(3):
    dr_group.add_variable(variable + "[%d]" %ii)

log_add_3vec("planet_rel_state.relstate_name.state.trans.position")
log_add_3vec("planet_rel_state.relstate_name.state.trans.velocity")
log_add_3vec("planet_rel_state.relstate_direct.state.trans.position")
log_add_3vec("planet_rel_state.relstate_direct.state.trans.velocity")


trick.add_data_record_group(dr_group, trick.DR_Buffer)
