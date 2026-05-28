dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always

def log_3vec(variable) :
  for ii in range(3) :
    dr_group.add_variable(variable + "[%d]" %ii)

def log_sep_state(ix):
  dr_group.add_variable("test_sep.sep_state_%d.active" %ii )
  log_3vec("test_sep.sep_state_%d.rel_state.trans.position" %ii )

for ii in [1,2] :
  log_sep_state(ii)

trick.add_data_record_group(dr_group, trick.DR_Buffer)
