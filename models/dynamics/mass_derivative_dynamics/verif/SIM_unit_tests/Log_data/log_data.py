drg = trick.sim_services.DRAscii("test_data")
drg.set_cycle(1.0)
drg.freq = trick.sim_services.DR_Always
trick.add_data_record_group(drg, trick.DR_Buffer)

def add_3array(drg, var):
  for ii in range (3):
    drg.add_variable( var + "[" + str(ii) + "]")

def add_3x3array(drg, var):
  for ii in range (3):
    add_3array( drg, var + "[" + str(ii) + "]")

def add_mass_outputs():
  drg.add_variable("test.mass")
  add_3array( drg, "test.test_dyn.pseudo_torque")
  add_3array( drg, "test.test_dyn.pseudo_force")

def add_others():
  add_3array( drg, "test.body.composite_body.state.rot.ang_vel_this")
  add_3array( drg, "test.R_nozzle")
  add_3array( drg, "test.R_propellant")
  add_3array( drg, "test.body.composite_body.state.trans.position")
  add_3x3array( drg, "test.inertia")
