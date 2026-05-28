def log_add_3vec( drg, var):
  for ii in range (0,3):
    drg.add_variable( var+ "[" + str(ii) + "]")

def log_add_3x3( drg, var):
  for jj in range (0,3):
    log_add_3vec( drg, var+ "[" + str(jj) + "]")

dr_group = trick.DRAscii("prs_verif")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable("prs_verif.earth_rel.state.sphere_coords.longitude", "prs_verif.earth_rel.longitude")
dr_group.add_variable("prs_verif.earth_rel.state.sphere_coords.latitude",  "prs_verif.earth_rel.geocentric_latitude")
dr_group.add_variable("prs_verif.earth_rel.state.ellip_coords.latitude",   "prs_verif.earth_rel.geodetic_latitude")
dr_group.add_variable("prs_verif.earth_rel.topocentric_altitude",          "prs_verif.earth_rel.geocentric_altitude")
dr_group.add_variable("prs_verif.earth_rel.state.ellip_coords.altitude",   "prs_verif.earth_rel.geodetic_altitude")
for ii in range(0,3) :
  dr_group.add_variable("prs_verif.earth_rel.state.cart_coords[" + str(ii) + "]", "prs_verif.earth_rel.relative_pos[" + str(ii) + "]" )

#flags
dr_group.add_variable("prs_verif.earth_rel.active")
dr_group.add_variable("prs_verif.earth_rel.calc_rel_vel")
dr_group.add_variable("prs_verif.earth_rel.calc_topocentric")
dr_group.add_variable("prs_verif.earth_rel.calc_topodetic")
dr_group.add_variable("prs_verif.earth_rel.calc_hang_roll")
dr_group.add_variable("prs_verif.earth_rel.calc_boost_reference")
dr_group.add_variable("prs_verif.earth_rel.calc_plumbline")
dr_group.add_variable("prs_verif.earth_rel.calc_range_safety")

# Orientation specifiers
log_add_3vec( dr_group, "prs_verif.earth_rel.hang_angle_body_vec")
log_add_3vec( dr_group, "prs_verif.earth_rel.roll_wrt_hdg_body_vec")

# reference values:
dr_group.add_variable("prs_verif.earth.r_eq")
dr_group.add_variable("prs_verif.earth.e_ellip_sq")

log_add_3vec( dr_group, "prs_verif.body.composite_body.state.trans.position")
log_add_3vec( dr_group, "prs_verif.earth.pfix.state.trans.position")
log_add_3vec( dr_group, "prs_verif.earth.pfix.state.rot.ang_vel_this")
log_add_3x3(  dr_group, "prs_verif.body.mass.composite_properties.T_parent_this")

dr_group.add_variable("prs_verif.earth_rel.br_ref_longitude")
dr_group.add_variable("prs_verif.earth_rel.br_ref_geodetic_latitude")
dr_group.add_variable("prs_verif.earth_rel.pl_ref_longitude")
dr_group.add_variable("prs_verif.earth_rel.pl_ref_geodetic_latitude")
dr_group.add_variable("prs_verif.earth_rel.pl_ref_azimuth")

# range variables
for range_type in ["launch_range", "landing_range", "entry_range"]:
  range_root = "prs_verif.earth_rel." + range_type
  dr_group.add_variable( range_root + ".totalrange_angle")
  dr_group.add_variable( range_root + ".crossrange_angle")
  dr_group.add_variable( range_root + ".downrange_angle")
  dr_group.add_variable( range_root + ".cross_range_avg_rad")
  dr_group.add_variable( range_root + ".down_range_avg_rad")
  dr_group.add_variable( range_root + ".total_range_avg_rad")
  dr_group.add_variable( range_root + ".cross_range_ref_rad")
  dr_group.add_variable( range_root + ".down_range_ref_rad")
  dr_group.add_variable( range_root + ".total_range_ref_rad")

# topocentric altitude - radial altitude above the reference ellipsoid
dr_group.add_variable("prs_verif.earth_rel.topocentric_altitude")

# range safety
dr_group.add_variable("prs_verif.earth_rel.range_safety.XVRT_Azi")
dr_group.add_variable("prs_verif.earth_rel.range_safety.YVRT_Azi")
dr_group.add_variable("prs_verif.earth_rel.range_safety.slant_range")
dr_group.add_variable("prs_verif.earth_rel.range_safety.pad_azimuth")
dr_group.add_variable("prs_verif.earth_rel.range_safety.XVRT")
dr_group.add_variable("prs_verif.earth_rel.range_safety.YVRT")
dr_group.add_variable("prs_verif.earth_rel.range_safety.ZVRT")

log_add_3vec( dr_group, "prs_verif.earth_rel.range_safety.XPad")
log_add_3vec( dr_group, "prs_verif.earth_rel.range_safety.VPad")
log_add_3vec( dr_group, "prs_verif.earth_rel.range_safety.NPad")
log_add_3vec( dr_group, "prs_verif.earth_rel.range_safety.XVP")

log_add_3x3(  dr_group, "prs_verif.earth_rel.range_safety.T_pfix_pad")
log_add_3x3(  dr_group, "prs_verif.earth_rel.range_safety.T_pfix_pad_SEU")

# topodetic / topocentric values
for topo_type in ["topocentric", "topodetic"]:
  topo_root = "prs_verif.earth_rel." + topo_type
  log_add_3x3(  dr_group, topo_root + ".T_pfix_to_this")
  log_add_3x3(  dr_group, topo_root + ".T_inrtl_to_this")
  log_add_3x3(  dr_group, topo_root + ".T_this_to_body")

  log_add_3vec( dr_group, topo_root + ".E_this_to_body_YPR")
  log_add_3vec( dr_group, topo_root + ".relative_vel.velocity")
  log_add_3vec( dr_group, topo_root + ".inertial_vel.velocity")

  dr_group.add_variable( topo_root + ".relative_vel.vel_xy")
  dr_group.add_variable( topo_root + ".relative_vel.flight_path")
  dr_group.add_variable( topo_root + ".relative_vel.azimuth")
  dr_group.add_variable( topo_root + ".relative_vel.altitude_rate")
  dr_group.add_variable( topo_root + ".inertial_vel.vel_xy")
  dr_group.add_variable( topo_root + ".inertial_vel.flight_path")
  dr_group.add_variable( topo_root + ".inertial_vel.azimuth")
  dr_group.add_variable( topo_root + ".inertial_vel.altitude_rate")

dr_group.add_variable("prs_verif.earth_rel.inrtl_vel_mag")
dr_group.add_variable("prs_verif.earth_rel.relative_vel_mag")
dr_group.add_variable("prs_verif.earth_rel.hang_angle")
dr_group.add_variable("prs_verif.earth_rel.roll_wrt_heading")

log_add_3vec( dr_group, "prs_verif.earth_rel.relative_vel")
log_add_3vec( dr_group, "prs_verif.earth_rel.pfix_rel_vel")
log_add_3vec( dr_group, "prs_verif.earth_rel.relative_accel")
log_add_3vec( dr_group, "prs_verif.earth_rel.E_br_body_YPR")
log_add_3vec( dr_group, "prs_verif.earth_rel.E_pl_body_PYR")

log_add_3x3(  dr_group,"prs_verif.earth_rel.T_inrtl_br")
log_add_3x3(  dr_group,"prs_verif.earth_rel.T_inrtl_pl")
