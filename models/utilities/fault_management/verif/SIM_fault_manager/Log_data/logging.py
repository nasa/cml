import trick

def make_dr_group(log_cycle) :
   dr_group = trick.DRAscii("fault_variables")
   dr_group.thisown = 0
   dr_group.set_cycle(log_cycle)
   dr_group.set_freq(trick.DR_Always)
   dr_group.enable()
   return dr_group


def add_replaced_variables(dr_group) :
   dr_group.add_variable("test_object.var_char_numeric")
   dr_group.add_variable("test_object.var_uchar")
   dr_group.add_variable("test_object.var_sint")
   dr_group.add_variable("test_object.var_usint")
   dr_group.add_variable("test_object.var_int")
   dr_group.add_variable("test_object.var_uint")
   dr_group.add_variable("test_object.var_lint")
   dr_group.add_variable("test_object.var_ulint")
   dr_group.add_variable("test_object.var_llint")
   dr_group.add_variable("test_object.var_ullint")
   dr_group.add_variable("test_object.var_float")
   dr_group.add_variable("test_object.var_double")
   return


def log_default(log_cycle) :
   dr_group = make_dr_group(log_cycle)

   add_replaced_variables(dr_group)

   trick.add_data_record_group(dr_group, trick.DR_Buffer)
   return


def log_bool(log_cycle) :
   dr_group = make_dr_group(log_cycle)

   add_replaced_variables(dr_group)
   dr_group.add_variable("test_object.var_bool")

   trick.add_data_record_group(dr_group, trick.DR_Buffer)
   return


def log_no_int(log_cycle) :
   dr_group = make_dr_group(log_cycle)

   dr_group.add_variable("test_object.var_float")
   dr_group.add_variable("test_object.var_double")

   trick.add_data_record_group(dr_group, trick.DR_Buffer)
   return
