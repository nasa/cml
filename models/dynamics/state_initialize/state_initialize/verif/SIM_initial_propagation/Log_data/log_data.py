import math
def log_add_3_vec( drg, var):
  for ii in range(3):
    drg.add_variable( var + "[" + str(ii) + "]" )

def log_add_3x3( drg, var):
  for ii in range(3):
    log_add_3_vec( drg, var + "[" + str(ii) + "]" )

def log_test_data ( log_cycle ) :
  dr_group = trick.DRAscii("test_data")
  dr_group.thisown = 0
  print(f"log_cycle: {log_cycle}")
  # set_cycle cannot be passed 0.0
  if (math.isclose(log_cycle, 0.0)):
      dr_group.set_cycle(1.0)
  else:
      dr_group.set_cycle(log_cycle)
  dr_group.freq = trick.DR_Always
  trick.add_data_record_group(dr_group, trick.DR_Buffer)

  dr_group.add_variable("test.prop_time")
  log_add_3_vec( dr_group, "test.body.composite_body.state.trans.position")
  log_add_3_vec( dr_group, "test.body.composite_body.state.trans.velocity")
  log_add_3x3( dr_group, "test.body.composite_body.state.rot.T_parent_this")

  return dr_group

# Useful for debugging error caused by gravity model
def log_test_data_debug ( log_cycle ) :
  dr_group = log_test_data(log_cycle)
  log_add_3_vec( dr_group, "test.body.grav_interaction.grav_accel")
  log_add_3x3(   dr_group, "earth.planet.pfix.state.rot.T_parent_this")
