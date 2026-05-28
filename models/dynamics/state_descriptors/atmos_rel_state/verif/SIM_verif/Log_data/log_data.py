def log_three_vector(dr_group, variable_name) :
   for ii in range(0,3) :
      full_name = variable_name + "[" + str(ii) + "]"
      dr_group.add_variable(full_name)

def log_three_matrix(dr_group, variable_name) :
   for ii in range(0,3) :
      full_name = variable_name + "[" + str(ii) + "]"
      log_three_vector(dr_group, full_name)


dr_group = trick.DRAscii("atmos_rel_verif")
dr_group.thisown = 0
dr_group.set_cycle(1.0)
dr_group.enable()

dr_group.add_variable("atmos_rel_verif.atmos_rel.free_stream_vel_mag")

log_three_vector(dr_group, "atmos_rel_verif.atmos_rel.free_stream_vel")
log_three_vector(dr_group, "atmos_rel_verif.atmos_rel.free_stream_td_vel")
log_three_vector(dr_group, "atmos_rel_verif.atmos_rel.free_stream_body_vel")

dr_group.add_variable("atmos_rel_verif.atmos_rel.free_stream_flight_path")
dr_group.add_variable("atmos_rel_verif.atmos_rel.free_stream_azimuth")
dr_group.add_variable("atmos_rel_verif.atmos_rel.angle_of_attack")
dr_group.add_variable("atmos_rel_verif.atmos_rel.angle_of_sideslip")
dr_group.add_variable("atmos_rel_verif.atmos_rel.bank_angle")
dr_group.add_variable("atmos_rel_verif.atmos_rel.total_angle_of_attack")
dr_group.add_variable("atmos_rel_verif.atmos_rel.phi_roll")
dr_group.add_variable("atmos_rel_verif.atmos_rel.mach_number")
dr_group.add_variable("atmos_rel_verif.atmos_rel.dynamic_pressure")
dr_group.add_variable("atmos_rel_verif.atmos_rel.drag_accel")
dr_group.add_variable("atmos_rel_verif.atmos_rel.side_accel")
dr_group.add_variable("atmos_rel_verif.atmos_rel.lift_accel")
dr_group.add_variable("atmos_rel_verif.atmos_rel.sensed_accel_mag")
dr_group.add_variable("atmos_rel_verif.atmos_rel.angle_of_attack_alt")
dr_group.add_variable("atmos_rel_verif.atmos_rel.angle_of_sideslip_alt")
dr_group.add_variable("atmos_rel_verif.atmos_rel.total_angle_of_attack_alt")
dr_group.add_variable("atmos_rel_verif.atmos_rel.phi_roll_alt")
dr_group.add_variable("atmos_rel_verif.atmos_rel.reynolds_number")

dr_group.add_variable("atmos_rel_verif.atmos_rel.qalpha_total_psfdeg")
dr_group.add_variable("atmos_rel_verif.atmos_rel.qalpha_total")
dr_group.add_variable("atmos_rel_verif.atmos_rel.qalpha")
dr_group.add_variable("atmos_rel_verif.atmos_rel.qbeta")

log_three_matrix(dr_group, "atmos_rel_verif.atmos_rel.T_inrtl_traj")
log_three_matrix(dr_group, "atmos_rel_verif.atmos_rel.T_traj_body")
log_three_matrix(dr_group, "atmos_rel_verif.atmos_rel.T_traj_wind")
log_three_matrix(dr_group, "atmos_rel_verif.atmos_rel.T_inrtl_wind")
log_three_matrix(dr_group, "atmos_rel_verif.atmos_rel.T_wind_stab")
log_three_matrix(dr_group, "atmos_rel_verif.atmos_rel.T_inrtl_stab")

trick.add_data_record_group(dr_group, trick.DR_Buffer)
