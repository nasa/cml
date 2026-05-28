import trick

def log_default(log_cycle) :
  dr_group = trick.DRAscii("fault_variables")
  dr_group.thisown = 0
  dr_group.set_cycle(log_cycle)
  dr_group.set_freq(trick.DR_Always)
  dr_group.enable()
  dr_group.add_variable("test_object.var")
  trick.add_data_record_group(dr_group, trick.DR_Buffer)
