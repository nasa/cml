dr_group = trick.DRAscii("vents")
dr_group.set_cycle(1.0)
dr_group.set_freq(trick.DR_Always)
trick.add_data_record_group(dr_group, trick.DR_Buffer)

def log_add_3_vec(drg, var):
  for ii in range(3):
    drg.add_variable(var + "[%d]" %ii)

dr_group.add_variable("ventset_obj.root_body.mass.composite_properties.mass")
dr_group.add_variable("ventset_obj.tank[0].dynamic_properties.consumable_mass")
dr_group.add_variable("ventset_obj.tank[1].dynamic_properties.consumable_mass")
log_add_3_vec(dr_group, "ventset_obj.set_2_tanks.force")
log_add_3_vec(dr_group, "ventset_obj.set_2_tanks.torque")

def log_add_state( body="ventset_obj.root_body"):
  log_add_3_vec(dr_group, body+".composite_body.state.trans.velocity")
  log_add_3_vec(dr_group, body+".composite_body.state.rot.ang_vel_this")

def log_add_activity( vent="ventset_obj.extra_vent"):
  dr_group.add_variable(vent+".venting")
