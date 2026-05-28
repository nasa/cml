drg_params = trick.DRAscii("vents")
drg_params.set_cycle(1.0)
drg_params.set_freq(trick.DR_Always)
trick.add_data_record_group(drg_params, trick.DR_Buffer)

def log_add_3vec(drg, var):
  for ii in range(3):
    drg.add_variable(var + "[%d]" %ii)

def log_simple_params( vent="ventset_obj.extra_vent"):
  drg_params.add_variable(vent+".force_mag")
  drg_params.add_variable(vent+".impulse_mag")
  drg_params.add_variable(vent+".duration")
  drg_params.add_variable(vent+".stop_time")
  log_add_3vec( drg_params, vent+".direction")
  log_add_3vec( drg_params, vent+".force")
  log_add_3vec( drg_params, vent+".impulse")


def log_full_params( vent="ventset_obj.extra_vent"):
  log_simple_params(vent)
  drg_params.add_variable(vent+".flowrate")
  drg_params.add_variable(vent+".exhaust_speed")

def log_flags( vent="ventset_obj.extra_vent"):
  drg_params.add_variable(vent+".direction_set")
  drg_params.add_variable(vent+".duration_set")
  drg_params.add_variable(vent+".force_mag_set")
  drg_params.add_variable(vent+".force_set")
  drg_params.add_variable(vent+".impulse_mag_set")
  drg_params.add_variable(vent+".impulse_set")
  drg_params.add_variable(vent+".flowrate_set")
  drg_params.add_variable(vent+".exhaust_set")

def log_mode():
  drg_params.add_variable("ventset_obj.vent0_apply_as_impulse")
  drg_params.add_variable("ventset_obj.vent1_apply_as_impulse")
  drg_params.add_variable("ventset_obj.extra_vent.apply_as_impulse")
  drg_params.add_variable("ventset_obj.extra_simple_vent.apply_as_impulse")
