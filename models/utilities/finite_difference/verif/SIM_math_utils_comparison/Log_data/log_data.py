dr_grp = trick.DRAscii("scalar_data")
dr_grp.set_cycle(1.0)
dr_grp.freq = trick.DR_Always
trick.add_data_record_group(dr_grp, trick.DR_Buffer)

dr_grp.add_variable("fd_obj.time")
dr_grp.add_variable("fd_obj.in_scalar")
dr_grp.add_variable("fd_obj.out_scalar_deriv1")
dr_grp.add_variable("fd_obj.out_math_utils")
dr_grp.add_variable("fd_obj.out_scalar_deriv2")
dr_grp.add_variable("fd_obj.error_deriv1")
dr_grp.add_variable("fd_obj.error_mu2")
dr_grp.add_variable("fd_obj.error_mu")
dr_grp.add_variable("fd_obj.error_deriv2")


def log_3vec(drg, var):
  for ii in range(3):
    drg.add_variable( "%s[%d]" %(var,ii))

dr_grp = trick.DRAscii("vector_data")
dr_grp.set_cycle(1.0)
dr_grp.freq = trick.DR_Always
trick.add_data_record_group(dr_grp, trick.DR_Buffer)

dr_grp.add_variable("fd_obj.time")
log_3vec(dr_grp,"fd_obj.in_vec3")
log_3vec(dr_grp,"fd_obj.out_vec3_deriv1")
log_3vec(dr_grp,"fd_obj.out_vec3_deriv2")
log_3vec(dr_grp,"fd_obj.error_deriv1_vec")
log_3vec(dr_grp,"fd_obj.error_deriv2_vec")


def log_3x3mx(drg,var):
  for ii in range(3):
    log_3vec(drg, "%s[%d]" %(var,ii))

dr_grp = trick.DRAscii("matrix_data")
dr_grp.set_cycle(1.0)
dr_grp.freq = trick.DR_Always
trick.add_data_record_group(dr_grp, trick.DR_Buffer)

dr_grp.add_variable("fd_obj.time")
log_3x3mx(dr_grp,"fd_obj.in_mat3x3")
log_3x3mx(dr_grp,"fd_obj.out_mat3x3_deriv1")
log_3x3mx(dr_grp,"fd_obj.error_deriv1_mx")
