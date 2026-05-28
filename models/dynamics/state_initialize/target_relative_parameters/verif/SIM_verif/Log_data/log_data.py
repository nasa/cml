drg = trick.sim_services.DRAscii("test_data")
drg.set_cycle(1.0)
drg.freq = trick.sim_services.DR_Always
trick.add_data_record_group(drg, trick.DR_Buffer)

def log_3vec( drg, var):
  for ii in range(3):
    drg.add_variable( var + "[%d]" %ii)

def log_params( drg, set_):
  drg.add_variable( set_ + ".altitude")
  drg.add_variable( set_ + ".theta_Rng")
  drg.add_variable( set_ + ".theta_Rot")
  drg.add_variable( set_ + ".phi_Cross")
  drg.add_variable( set_ + ".Vmag")
  drg.add_variable( set_ + ".gamma")
  drg.add_variable( set_ + ".Lambda")


#if (test.framework_provides_R3):
log_3vec( drg, "test.position_in")
log_3vec( drg, "test.position_out_from_rot")
log_3vec( drg, "test.position_out_from_pos_vel")
log_3vec( drg, "test.position_out")
log_3vec( drg, "test.position_out_from_cross")
log_3vec( drg, "test.velocity_in")
log_3vec( drg, "test.velocity_out")
log_3vec( drg, "test.velocity_out_from_pos_vel")
drg.add_variable("test.tr_params_to_R3.target_is_behind_vehicle")
drg.add_variable("test.tr_params_to_R3.target_is_behind_vehicle_check")

#else:
log_params( drg, "test.params_in")
drg.add_variable( "test.pos_angle_in")
log_params( drg, "test.params_out")
