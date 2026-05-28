def log_3vec( drg, var):
  for ii in range(3) :
    drg.add_variable(var + "[%d]" %ii)
def log_3x3( drg, var):
  for ii in range(3) :
    log_3vec( drg, var + "[%d]" %ii)

dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

log_3vec( dr_group, "verif.corr_state.pos_error")
log_3vec( dr_group, "verif.dispersed_position")
log_3vec( dr_group, "verif.corr_state.vel_error")
log_3vec( dr_group, "verif.dispersed_velocity")


def log_att():
  log_3vec( dr_group, "verif.corr_state.att_error")
  log_3x3( dr_group, "verif.dispersed_T_mx")

def log_prm():
  for ii in range(5) :
    dr_group.add_variable("verif.corr_state.prm_error[%d]" %ii)
