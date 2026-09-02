dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always

def log_3vec(variable) :
  for ii in range(3) :
    dr_group.add_variable(f"{variable}[{ii}]")

def log_sep_state(ix):
  dr_group.add_variable(f"test_sep.sep_state_{ii}.active")
  log_3vec(f"test_sep.sep_state_{ii}.rel_state.trans.position")

for ii in [1,2] :
  log_sep_state(ii)

trick.add_data_record_group(dr_group, trick.DR_Buffer)
