drg = trick.DRAscii("test_data")
drg.set_cycle(10)
drg.freq = trick.DR_Always

def log_add_veh_state( veh) :
  log_add_3vec(drg, veh+".body.composite_body.state.trans.position")
  log_add_3vec(drg, veh+".body.composite_body.state.trans.velocity")
  drg.add_variable( veh+".planet_rel_state.state.ellip_coords.altitude")
  drg.add_variable( veh+".planet_rel_state.state.ellip_coords.latitude")
  drg.add_variable( veh+".planet_rel_state.state.ellip_coords.longitude")

def log_add_veh_atmos( veh) :
  drg.add_variable( veh+".atmos_exec.out.density")
  drg.add_variable( veh+".atmos_exec.out.pressure")
  drg.add_variable( veh+".atmos_exec.out.temperature")
  drg.add_variable( veh+".atmos_exec.out.speed_of_sound")
  drg.add_variable( veh+".atmos_exec.out.dynamic_viscosity")
  drg.add_variable( veh+".atmos_exec.out.mean_free_path")
  drg.add_variable( veh+".atmos_exec.out.atmos_new_time")

def log_add_veh_wind( veh) :
  drg.add_variable( veh+".atmos_exec.out.wind_angle_blowing_from")
  drg.add_variable( veh+".atmos_exec.out.wind_angle_blowing_to")
  drg.add_variable( veh+".atmos_exec.out.wind_vmag")
  log_add_3vec(drg, veh+".atmos_exec.out.wind_velocity_tc")
  log_add_3vec(drg, veh+".atmos_exec.out.wind_velocity_td")
  log_add_3vec(drg, veh+".atmos_exec.out.wind_velocity_eci")


log_add_veh_state("veh1")
log_add_veh_atmos("veh1")
log_add_veh_wind( "veh1")

log_add_veh_state("veh2")
log_add_veh_atmos("veh2")
log_add_veh_wind( "veh2")

trick.add_data_record_group(drg, trick.DR_Buffer)

# Add the master veh to the data recording group test_data
# Kept separate since most runs have veh_master disabled
def log_append_veh (veh) :
  dr_group = trick.get_data_record_group("test_data")
  log_add_veh_state( veh)
  log_add_veh_atmos( veh)
