def log_constraint(var, n):
  drg = trick.DRAscii(var)
  drg.thisown = False
  drg.set_cycle(0.1)
  drg.freq = trick.DR_Always
  trick.add_data_record_group(drg, trick.DR_Buffer)
  drg.add_variable("test.var_i")
  drg.add_variable("test.constraints."+var+".violation_count")
  drg.add_variable("test.constraints."+var+".violated")
  for ii in range(n):
    drg.add_variable("test.constraints."+var+".tests[%d].violation" %ii)

def log_constraint_t(var, n):
  drg = trick.DRAscii(var)
  drg.thisown = False
  drg.set_cycle(0.1)
  drg.freq = trick.DR_Always
  trick.add_data_record_group(drg, trick.DR_Buffer)
  drg.add_variable("test.var_i")
  drg.add_variable("test.constraints."+var+".violation_count")
  drg.add_variable("test.constraints."+var+".violated")
  for ii in range(n):
    drg.add_variable("test.constraints."+var+".tests[%d].violation" %ii)
  for ii in range(n):
    drg.add_variable("test.constraints."+var+".tests[%d].violation_timer" %ii)
  drg.add_variable("test.constraints."+var+".test_violated_time_limit")
  drg.add_variable("test.constraints."+var+".test_violated_index")


