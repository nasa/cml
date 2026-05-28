def add_3array(drg, var):
  for ii in range (3):
    drg.add_variable( var + "[%d]" %ii)

def add_3x3array(drg, var):
  for ii in range (3):
    add_3array( drg, var + "[%d]" %ii)

####################################################
drg1 = trick.sim_services.DRAscii("basic_data")
drg1.set_cycle(0.1)
drg1.freq = trick.sim_services.DR_Always
trick.add_data_record_group(drg1, trick.DR_Buffer)

drg1.add_variable("vehicle.body.composite_body.state.rot.ang_vel_this[2]")
drg1.add_variable("vehicle.body.mass.composite_properties.mass")
drg1.add_variable("vehicle.decay_time")

def log_basic_addon():
    drg1.add_variable("vehicle.body.composite_body.state.trans.position[0]")
    drg1.add_variable("vehicle.body.composite_body.state.trans.position[1]")
    drg1.add_variable("vehicle.body.composite_body.state.trans.velocity[0]")
    drg1.add_variable("vehicle.body.composite_body.state.trans.velocity[1]")
    add_3array(drg1,"vehicle.test_dyn.pseudo_force")
    add_3array(drg1,"vehicle.body.derivs.trans_accel")
    drg1.add_variable("vehicle.eigen_rot")



# Create a second data set for more general support; these files are not
# committed to the repo and not considered regression data.
drg2 = trick.sim_services.DRAscii("support_data")
drg2.set_cycle(0.1)
drg2.freq = trick.sim_services.DR_Always
trick.add_data_record_group(drg2, trick.DR_Buffer)

drg2.add_variable("vehicle.body.mass.composite_properties.mass")
drg2.add_variable("vehicle.tank.composite_properties.mass")
add_3array( drg2, "vehicle.test_dyn.pseudo_torque")
add_3array( drg2, "vehicle.test_dyn.pseudo_force")
add_3array( drg2, "vehicle.body.composite_body.state.rot.ang_vel_this")
add_3array( drg2, "vehicle.body.composite_body.state.trans.position")
add_3array( drg2, "vehicle.body.composite_body.state.trans.velocity")
add_3x3array( drg2, "vehicle.body.mass.composite_properties.inertia")
add_3array( drg2, "vehicle.body.derivs.rot_accel")
