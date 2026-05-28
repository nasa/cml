def add_3vec( drg, var):
  for ii in range(0,3) :
    drg.add_variable(var +"[" + str(ii) + "]" )



dr_group = trick.sim_services.DRAscii("test_data")
dr_group.set_cycle(100000.0)
dr_group.freq = trick.sim_services.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)


add_3vec( dr_group,"earth.planet.inertial.state.trans.position")
add_3vec( dr_group,"earth.planet.inertial.state.trans.velocity")
add_3vec( dr_group,"moon.planet.inertial.state.trans.position")
add_3vec( dr_group,"moon.planet.inertial.state.trans.velocity")
