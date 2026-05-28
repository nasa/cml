def log_three_vector(dr_group, variable_name) :
   for ii in range(0,3) :
      full_name = variable_name + "[" + str(ii) + "]"
      dr_group.add_variable(full_name)

def log_three_matrix(dr_group, variable_name) :
   for ii in range(0,3) :
      full_name = variable_name + "[" + str(ii) + "]"
      log_three_vector(dr_group, full_name)


dr_group = trick.DRAscii("atmos_test_verif")
dr_group.thisown = 0
dr_group.set_cycle(1.0)
dr_group.enable()

log_three_vector(dr_group, "atmos_test_verif.atmos_test.ryp_vector")
log_three_matrix(dr_group, "atmos_test_verif.atmos_test.T_traj_body")

trick.add_data_record_group(dr_group, trick.DR_Buffer)
