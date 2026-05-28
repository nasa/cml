####################################TRICK HEADER####################################
### PURPOSE: (To define the elements to be logged in the logging of mutil_verif) ###
####################################################################################

def log_add_nvec(drg, var, num):
  for ii in range(num):
    drg.add_variable( var+"[%d]" %ii)

def log_add_nxm_mx( drg, var, n, m):
  for ii in range(n):
    log_add_nvec( drg, var+"[%d]" %ii, m)

def new_dr_group(name):
  drg = trick.DRAscii(name)
  drg.thisown = 0
  drg.set_cycle(1.0)
  drg.freq = trick.DR_Always
  trick.add_data_record_group(drg, trick.DR_Buffer)
  return drg

def log_divide_protected():
  dr_group = new_dr_group( "divide_protected")
  log_add_nvec( dr_group, "ut_private.div_protected_vals_double", 5)
  log_add_nvec( dr_group, "ut_private.div_protected_vals_float", 4)

def log_acos_protected():
  dr_group = new_dr_group("acos_protected")
  log_add_nvec(  dr_group, "ut_private.acos_protected_vals_double", 3)
  log_add_nvec(  dr_group, "ut_private.acos_protected_vals_float", 3)

def log_asin_protected():
  dr_group = new_dr_group("asin_protected")
  log_add_nvec(  dr_group, "ut_private.asin_protected_vals_double",3)
  log_add_nvec(  dr_group, "ut_private.asin_protected_vals_float", 3)

def log_sqrt_protected():
  dr_group = new_dr_group("sqrt_protected")
  log_add_nvec(  dr_group, "ut_private.sqrt_protected_vals_double",4)
  log_add_nvec(  dr_group, "ut_private.sqrt_protected_vals_float", 2)

def log_log_protected():
  dr_group = new_dr_group("log_protected")
  log_add_nvec( dr_group, "ut_private.log_protected_vals_double", 4)
  log_add_nvec( dr_group, "ut_private.log_protected_vals_float",  4)

def log_log10_protected():
  dr_group = new_dr_group("log10_protected")
  log_add_nvec( dr_group, "ut_private.log10_protected_vals_double", 4)
  log_add_nvec( dr_group, "ut_private.log10_protected_vals_float",  4)

def log_is_near_equal():
  dr_group = new_dr_group( "is_near_equal")
  log_add_nvec( dr_group, "ut_private.is_near_equal_vals", 11)

def log_has_changed_from():
  dr_group = new_dr_group( "has_changed_from")
  log_add_nvec( dr_group, "ut_private.has_changed_from_vals", 6)

def log_is_equal():
  dr_group = new_dr_group( "is_equal")
  log_add_nvec( dr_group, "ut_private.is_equal_vals", 8)

def log_is_within_abs_tolerance():
  dr_group = new_dr_group( "is_within_abs_tolerance")
  log_add_nvec( dr_group, "ut_private.is_within_abs_tolerance_vals", 9)

def log_is_within_rel_tolerance():
  dr_group = new_dr_group( "is_within_rel_tolerance")
  log_add_nvec( dr_group, "ut_private.is_within_rel_tolerance_vals", 9)

def log_sign():
  dr_group = new_dr_group( "sign")
  log_add_nvec( dr_group, "ut_private.sign_vals", 3)

def log_polynomial():
  dr_group = new_dr_group("polynomial")
  log_add_nvec( dr_group, "ut_private.polynomial_vals", 3)

def log_quadratic_solver():
  dr_group = new_dr_group("quadratic_solver")
  log_add_nxm_mx( dr_group, "ut_private.quadratic_solver_vals", 13, 2)
