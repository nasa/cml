dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always

def log_3vec(variable) :
  for ii in range(3) :
    dr_group.add_variable(variable + "[%d]" %ii)

dr_group.add_variable("test_sep.sep_state.active" )
log_3vec("test_sep.sep_state.rel_state.trans.position" )
log_3vec("test_sep.sep_state.rel_state.trans.velocity" )
dr_group.add_variable("test_sep.sep_state.rel_state.rot.Q_parent_this.scalar" )
log_3vec("test_sep.sep_state.rel_state.rot.Q_parent_this.vector" )
log_3vec("test_sep.sep_state.rel_state.rot.ang_vel_this" )
dr_group.add_variable("test_sep.sep_state.separation_distance" )
log_3vec("test_sep.sep_state.rel_orientation.euler_angles" )

trick.add_data_record_group(dr_group, trick.DR_Buffer)
