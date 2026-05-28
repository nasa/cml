def log_add_3vec(drg, var):
  for ii in range(3):
    drg.add_variable( var+"[%d]" %ii)

def log_add_effect( drg, effect):
  drg.add_variable( effect + ".offset_trail_dist")
  drg.add_variable( effect + ".offset_radial_dist")
  drg.add_variable( effect + ".in_region")
  drg.add_variable( effect + ".force_toward_A")

def log_add_instance(drg, instance):
  drg.add_variable( instance+".objectB.trailing_distance")
  drg.add_variable( instance+".objectB.radial_distance")
  log_add_effect( drg, instance+".prf_model")
  drg.add_variable(    instance+".prf_model.PRF")
  log_add_effect( drg, instance+".rev_flow")
  drg.add_variable(    instance+".rev_flow.Qrev")

dr_group = trick.DRAscii("wake_effects_unit_test")
dr_group.set_cycle(1.0)
dr_group.enable()
trick.add_data_record_group(dr_group, trick.DR_Buffer)


dr_group.add_variable("test.freestream_mach")
dr_group.add_variable( "test.wake_with_force.objectB.separation_distance")
dr_group.add_variable( "test.wake_with_force.objectB.axial_sep_vel")

log_add_instance(dr_group, "test.wake_with_force")

log_add_3vec( dr_group,"test.wake_with_force.objectB.force")
