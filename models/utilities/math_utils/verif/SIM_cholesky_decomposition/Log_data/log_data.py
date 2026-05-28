def log_arr( drg, var, size):
  for ii in range (size):
    drg.add_variable( var+"["+str(ii)+"]")

def log_arr_2( drg, var, size1, size2):
  for ii in range( size1):
    log_arr( drg, var+"["+str(ii)+"]", size2)

dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always

dr_group.add_variable( "simple_so.sub_mx")
log_arr_2( dr_group, "simple_so.A", 10, 10)
log_arr_2( dr_group, "simple_so.sqrt_A", 10, 10)

trick.add_data_record_group(dr_group, trick.DR_Buffer)
