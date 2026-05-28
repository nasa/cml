# Setup for data logging for pointing reference frame output
def add_3vec( drg, var):
  for ii in range(3) :
    drg.add_variable(var +"[" + str(ii) + "]" )
def add_3x3( drg, var):
  for jj in range(3):
    add_3vec( drg, var + "[" + str(jj) + "]")

dr_group = trick.sim_services.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.sim_services.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

add_3vec( dr_group,"vehicle.rel_state.rel_state.trans.position")
add_3vec( dr_group,"vehicle.rel_state.rel_state.trans.velocity")
dr_group.add_variable( "frame.pointing_ref_frame.pointing_frame.state.rot.ang_vel_this[2]")
add_3x3( dr_group, "frame.pointing_ref_frame.pointing_frame.state.rot.T_parent_this")
