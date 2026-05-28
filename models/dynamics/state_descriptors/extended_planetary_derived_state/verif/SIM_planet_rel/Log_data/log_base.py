#Log a vector
def log_add_3vec( drg, var):
  for ii in range (0,3):
    drg.add_variable( var+ "[%d]"%ii)

#Log a 3x3 matrix
def log_add_3x3( drg, var):
  for jj in range (0,3):
    log_add_3vec( drg, var+ "[%d]"%jj)

def new_group(name, cycle=5.0):
  dr_group = trick.DRAscii(name)
  dr_group.set_cycle(cycle)
  dr_group.freq = trick.DR_Always
  trick.add_data_record_group(dr_group, trick.DR_Buffer)
  return dr_group


#Log vehicle states
def log_vehicle_states():
  dr_group = new_group("vehicle_state")
  dr_group.add_variable("vehicle.planet_rel_state.state.sphere_coords.longitude")
  dr_group.add_variable("vehicle.planet_rel_state.state.sphere_coords.latitude")
  dr_group.add_variable("vehicle.planet_rel_state.state.sphere_coords.altitude")

  dr_group.add_variable("vehicle.planet_rel_state.state.ellip_coords.longitude")
  dr_group.add_variable("vehicle.planet_rel_state.state.ellip_coords.latitude")
  dr_group.add_variable("vehicle.planet_rel_state.state.ellip_coords.altitude")

  log_add_3vec( dr_group, "vehicle.planet_rel_state.state.cart_coords")
  log_add_3vec( dr_group, "vehicle.body.composite_body.state.trans.position")
  log_add_3vec( dr_group, "vehicle.body.composite_body.state.trans.velocity")
  log_add_3x3(  dr_group, "vehicle.body.mass.composite_properties.T_parent_this")

#Log submodule control flags
def log_control_flags(cycle=200):
  dr_group = new_group("control_flags", cycle)
  dr_group.add_variable("vehicle.planet_rel_state.calc_rel_vel")
  dr_group.add_variable("vehicle.planet_rel_state.calc_topocentric")
  dr_group.add_variable("vehicle.planet_rel_state.calc_topocentric_altitude")
  dr_group.add_variable("vehicle.planet_rel_state.calc_topodetic")
  dr_group.add_variable("vehicle.planet_rel_state.calc_pt_to_pt")
  dr_group.add_variable("vehicle.planet_rel_state.calc_hang_roll")
  dr_group.add_variable("vehicle.planet_rel_state.calc_boost_reference")
  dr_group.add_variable("vehicle.planet_rel_state.calc_plumbline")
  dr_group.add_variable("vehicle.planet_rel_state.using_launch_range")
  dr_group.add_variable("vehicle.planet_rel_state.using_landing_range")
  dr_group.add_variable("vehicle.planet_rel_state.using_entry_range")
  dr_group.add_variable("vehicle.planet_rel_state.calc_range_safety")

def log_sub_model_active_flags():
  dr_group = new_group("sub_model_flags")
  dr_group.add_variable("vehicle.planet_rel_state.launch_range.active")
  dr_group.add_variable("vehicle.planet_rel_state.landing_range.active")
  dr_group.add_variable("vehicle.planet_rel_state.entry_range.active")


#Log relative velocity
def log_relative_velocity():
  dr_group = new_group("rel_vel")
  log_add_3vec( dr_group, "earth.planet.pfix.state.trans.position")
  log_add_3vec( dr_group, "earth.planet.pfix.state.rot.ang_vel_this")
  log_add_3x3( dr_group, "earth.planet.pfix.state.rot.T_parent_this")
  dr_group.add_variable("vehicle.planet_rel_state.inrtl_vel_mag")
  dr_group.add_variable("vehicle.planet_rel_state.relative_vel_mag")
  log_add_3vec( dr_group, "vehicle.planet_rel_state.relative_vel")
  log_add_3vec( dr_group, "vehicle.planet_rel_state.pfix_rel_vel")

#Log NED frame
def log_ned_ref(topo_type, with_rel_vec, with_tc_altitude=False):
  dr_group = new_group("NED_"+topo_type)
  topo_root = "vehicle.planet_rel_state." + topo_type
  log_add_3x3(  dr_group, topo_root + ".T_pfix_to_this")
  log_add_3x3(  dr_group, topo_root + ".T_inrtl_to_this")

  log_add_3x3(  dr_group, topo_root + ".T_this_to_body")
  log_add_3vec( dr_group, topo_root + ".E_this_to_body_YPR")

  if (with_tc_altitude):
    dr_group.add_variable("vehicle.planet_rel_state.topocentric_altitude")


  if with_rel_vec:
    dr_group.add_variable( topo_root + ".relative_vel.vel_xy")
    dr_group.add_variable( topo_root + ".relative_vel.flight_path")
    dr_group.add_variable( topo_root + ".relative_vel.azimuth")
    dr_group.add_variable( topo_root + ".relative_vel.altitude_rate")

    dr_group.add_variable( topo_root + ".inertial_vel.vel_xy")
    dr_group.add_variable( topo_root + ".inertial_vel.flight_path")
    dr_group.add_variable( topo_root + ".inertial_vel.azimuth")
    dr_group.add_variable( topo_root + ".inertial_vel.altitude_rate")

    log_relative_velocity()

#Log topocentric frame
def log_topocentric_ref(with_rel_vec=False, with_altitude=False):
  log_ned_ref("topocentric", with_rel_vec, with_altitude)

#Log topodetic frame
def log_topodetic_ref(with_rel_vec=False):
  log_ned_ref("topodetic", with_rel_vec)

#Log point-to-point values
def log_pt_to_pt():
  dr_group = new_group("pt_to_pt")
  for jj in range(3):
    dr_group.add_variable("vehicle.planet_rel_state.state.cart_coords[%d]" %jj)
  for ii in range(6):
    for jj in range(3):
      dr_group.add_variable("vehicle.pt_to_pt_position[%d][%d]" %(ii,jj))

#Log hang and roll angles
def log_hang_roll_angles():
  dr_group = new_group("hang_roll")
  log_add_3vec( dr_group, "vehicle.planet_rel_state.hang_angle_body_vec")
  log_add_3vec( dr_group, "vehicle.planet_rel_state.roll_wrt_hdg_body_vec")

  dr_group.add_variable("vehicle.planet_rel_state.hang_angle")
  dr_group.add_variable("vehicle.planet_rel_state.roll_wrt_heading")

#Log boost reference frame
def log_boost_ref():
  dr_group = new_group("boost_ref")
  dr_group.add_variable("vehicle.planet_rel_state.br_ref_longitude")
  dr_group.add_variable("vehicle.planet_rel_state.br_ref_geodetic_latitude")

  log_add_3x3(  dr_group,"vehicle.planet_rel_state.T_inrtl_br")
  log_add_3vec( dr_group, "vehicle.planet_rel_state.E_br_body_YPR")

#Log plumbline reference frame
def log_plumbline_ref():
  dr_group = new_group("plumbline")
  dr_group.add_variable("vehicle.planet_rel_state.pl_ref_longitude")
  dr_group.add_variable("vehicle.planet_rel_state.pl_ref_geodetic_latitude")
  dr_group.add_variable("vehicle.planet_rel_state.pl_ref_azimuth")

  log_add_3x3(  dr_group,"vehicle.planet_rel_state.T_inrtl_pl")
  log_add_3vec( dr_group, "vehicle.planet_rel_state.E_pl_body_PYR")

#Log range
def log_range(range_type):
  dr_group = new_group("range_"+range_type)
  range_root = "vehicle.planet_rel_state." + range_type
  dr_group.add_variable( range_root + ".crossrange_angle")
  dr_group.add_variable( range_root + ".downrange_angle")

  dr_group.add_variable( range_root + ".totalrange_angle")
  dr_group.add_variable( range_root + ".crossrange_angle")
  dr_group.add_variable( range_root + ".downrange_angle")

  dr_group.add_variable( range_root + ".total_range_avg_rad")
  dr_group.add_variable( range_root + ".cross_range_avg_rad")
  dr_group.add_variable( range_root + ".down_range_avg_rad")

  dr_group.add_variable( range_root + ".total_range_ref_rad")
  dr_group.add_variable( range_root + ".cross_range_ref_rad")
  dr_group.add_variable( range_root + ".down_range_ref_rad")

#Log launch range
def log_launch_range():
  log_range("launch_range")

#Log landing range
def log_landing_range():
  log_range("landing_range")

#Log entry range
def log_entry_range():
  log_range("entry_range")

#Log range safety
def log_range_safety():
  dr_group = new_group("range_safety")
  dr_group.add_variable("vehicle.planet_rel_state.range_safety.XVRT_Azi")
  dr_group.add_variable("vehicle.planet_rel_state.range_safety.YVRT_Azi")

  log_add_3x3(  dr_group, "vehicle.planet_rel_state.range_safety.T_pfix_pad")
  log_add_3x3(  dr_group, "vehicle.planet_rel_state.range_safety.T_pfix_pad_SEU")

  dr_group.add_variable("vehicle.planet_rel_state.range_safety.XVRT")
  dr_group.add_variable("vehicle.planet_rel_state.range_safety.YVRT")
  dr_group.add_variable("vehicle.planet_rel_state.range_safety.ZVRT")

  log_add_3vec( dr_group, "vehicle.planet_rel_state.range_safety.XPad")
  log_add_3vec( dr_group, "vehicle.planet_rel_state.range_safety.VPad")
  log_add_3vec( dr_group, "vehicle.planet_rel_state.range_safety.NPad")

  dr_group.add_variable("vehicle.planet_rel_state.range_safety.pad_azimuth")
  log_add_3vec( dr_group, "vehicle.planet_rel_state.range_safety.XVP")

  dr_group.add_variable("vehicle.planet_rel_state.range_safety.slant_range")

  log_add_3vec( dr_group, "vehicle.body.derivs.trans_accel")
  log_add_3vec( dr_group, "vehicle.planet_rel_state.relative_accel")
