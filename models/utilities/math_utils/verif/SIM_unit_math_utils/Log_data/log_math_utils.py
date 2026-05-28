####################################TRICK HEADER####################################
### PURPOSE: (To define the elements to be logged in the logging of mutil_verif) ###
####################################################################################
def log_add_n_vec(drg, var, n):
  for ii in range(n):
    drg.add_variable( var+"[%d]" %ii)

def log_add_nxm_mx( drg, var, n, m):
  for ii in range(n):
    log_add_n_vec( drg, var+"[%d]" %ii, m)

def log_add_nxmxl_mx( drg, var, n, m, l):
  for ii in range(n):
    log_add_nxm_mx( drg, var+"[%d]" %ii, m, l)

def log_add_3vec(drg, var):
  log_add_n_vec( drg, var, 3)

def log_add_3x3mx(drg, var):
  log_add_nxm_mx( drg, var, 3, 3)


def log_inertial_to_lvlh():
  recording_group_name =  "inertial_to_lvlh"
  dr_group = trick.DRAscii(recording_group_name)
  dr_group.thisown = 0
  dr_group.set_cycle(1.0)
  dr_group.freq = trick.DR_Always

  log_add_3vec(  dr_group, "mutil_verif.vector1")
  log_add_3vec(  dr_group, "mutil_verif.vector2")
  log_add_3x3mx( dr_group, "mutil_verif.T_inrtl_lvlh")

  trick.add_data_record_group(dr_group, trick.DR_Buffer)
  return


def log_inertial_to_uvw():
  recording_group_name = "inertial_to_uvw"
  dr_group = trick.DRAscii(recording_group_name)
  dr_group.thisown = 0
  dr_group.set_cycle(1.0)
  dr_group.freq = trick.DR_Always

  log_add_3vec(  dr_group, "mutil_verif.vector1")
  log_add_3vec(  dr_group, "mutil_verif.vector2")
  log_add_3x3mx( dr_group, "mutil_verif.T_inrtl_uvw")

  trick.add_data_record_group(dr_group, trick.DR_Buffer)
  return


def log_inertial_to_reference():
  recording_group_name = "inertial_to_reference"
  dr_group = trick.DRAscii(recording_group_name)
  dr_group.thisown = 0
  dr_group.set_cycle(1.0)
  dr_group.freq = trick.DR_Always

  log_add_3vec(  dr_group, "mutil_verif.vector1")
  log_add_3vec(  dr_group, "mutil_verif.vector2")
  log_add_3x3mx( dr_group, "mutil_verif.T_inrtl_reference")

  trick.add_data_record_group(dr_group, trick.DR_Buffer)
  return

def log_inertial_to_vnc():
  recording_group_name = "inertial_to_vnc"
  dr_group = trick.DRAscii(recording_group_name)
  dr_group.thisown = 0
  dr_group.set_cycle(1.0)
  dr_group.freq = trick.DR_Always

  log_add_3vec(  dr_group, "mutil_verif.vector1")
  log_add_3vec(  dr_group, "mutil_verif.vector2")
  log_add_3x3mx( dr_group, "mutil_verif.T_inrtl_vnc")

  trick.add_data_record_group(dr_group, trick.DR_Buffer)
  return

def log_T_pfix_to_enu():
  recording_group_name = "T_pfix_to_enu"
  dr_group = trick.DRAscii(recording_group_name)
  dr_group.thisown = 0
  dr_group.set_cycle(1.0)
  dr_group.freq = trick.DR_Always

  log_add_3vec(  dr_group, "mutil_verif.vector1")
  log_add_3x3mx( dr_group, "mutil_verif.T_pfix_enu")

  trick.add_data_record_group(dr_group, trick.DR_Buffer)
  return

def log_Q_enu_to_pfix():
  recording_group_name = "Q_enu_to_pfix"
  dr_group = trick.DRAscii(recording_group_name)
  dr_group.thisown = 0
  dr_group.set_cycle(1.0)
  dr_group.freq = trick.DR_Always

  dr_group.add_variable("mutil_verif.latitude")
  dr_group.add_variable("mutil_verif.longitude")
  dr_group.add_variable("mutil_verif.Q_enu_pfix.scalar")
  log_add_3vec( dr_group, "mutil_verif.Q_enu_pfix.vector")

  trick.add_data_record_group(dr_group, trick.DR_Buffer)
  return

def log_unit_vec_deriv():
  recording_group_name = "unit_vec_deriv"
  dr_group = trick.DRAscii(recording_group_name)
  dr_group.thisown = 0
  dr_group.set_cycle(1.0)
  dr_group.freq = trick.DR_Always

  log_add_3vec( dr_group, "mutil_verif.vector1")
  log_add_3vec( dr_group, "mutil_verif.vector2")
  log_add_3vec( dr_group, "mutil_verif.unit_vec_deriv")

  trick.add_data_record_group(dr_group, trick.DR_Buffer)
  return

def log_backward_difference():
  recording_group_name = "compute_backward_difference"
  dr_group = trick.DRAscii(recording_group_name)
  dr_group.thisown = 0
  dr_group.set_cycle(1.0)
  dr_group.freq = trick.DR_Always

  for ii in range(0,5):
    dr_group.add_variable("mutil_verif.back_diff_vals[" + str(ii) + "]")
  dr_group.add_variable("mutil_verif.backward_diff")
  dr_group.add_variable("mutil_verif.size")

  trick.add_data_record_group(dr_group, trick.DR_Buffer)
  return

def log_matrices():
  dr_group = trick.DRAscii("matrices")
  dr_group.thisown = 0
  dr_group.set_cycle(1.0)
  dr_group.freq = trick.DR_Always
  trick.add_data_record_group(dr_group, trick.DR_Buffer)

  log_add_n_vec(    dr_group, "mutil_verif.vec8", 8)
  log_add_nxmxl_mx( dr_group, "mutil_verif.arr43", 2, 4, 3)
  log_add_nxmxl_mx( dr_group, "mutil_verif.arr42", 3, 4, 2)
  log_add_nxm_mx(   dr_group, "mutil_verif.arr34", 3, 4)
  log_add_nxmxl_mx( dr_group, "mutil_verif.arr33", 11, 3, 3)
  log_add_nxm_mx(   dr_group, "mutil_verif.arr32", 3, 2)
  log_add_nxm_mx(   dr_group, "mutil_verif.arr24", 2, 4)
  log_add_nxm_mx(   dr_group, "mutil_verif.arr23", 2, 3)
  log_add_nxmxl_mx( dr_group, "mutil_verif.arr22", 3, 2, 2)
  return

def log_pv_covariance_matrix():
  dr_group = trick.DRAscii("pv_covariance_matrix")
  dr_group.thisown = 0
  dr_group.set_cycle(1.0)
  dr_group.freq = trick.DR_Always
  trick.add_data_record_group(dr_group, trick.DR_Buffer)

  log_add_nxmxl_mx( dr_group, "mutil_verif.arr33", 1, 3, 3)
  log_add_nxmxl_mx( dr_group, "mutil_verif.arr66", 2, 6, 6)
  return

def log_correlation_coefficients():
  dr_group = trick.DRAscii("correlation_coefficients")
  dr_group.thisown = 0
  dr_group.set_cycle(1.0)
  dr_group.freq = trick.DR_Always
  trick.add_data_record_group(dr_group, trick.DR_Buffer)

  log_add_nxmxl_mx( dr_group, "mutil_verif.arr33", 2, 3, 3)
  return
