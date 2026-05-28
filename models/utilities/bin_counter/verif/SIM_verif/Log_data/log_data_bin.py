"""
  Logging data for Bin Counter type 
"""
drg = trick.sim_services.DRAscii("test_data")
drg.set_cycle(1.0)
drg.freq = trick.sim_services.DR_Always
trick.add_data_record_group(drg, trick.DR_Buffer)

for ii in range(3):
  drg.add_variable(f"test.bin_counter_vec.bin_data[{ii}].bin_floor")
  drg.add_variable(f"test.bin_counter_vec.bin_data[{ii}].count")
for ii in range(3):
  drg.add_variable(f"test.bin_counter_arr.bin_data[{ii}].bin_floor")
  drg.add_variable(f"test.bin_counter_arr.bin_data[{ii}].count")
for ii in range(5):
  drg.add_variable(f"test.bin_counter_arr_open.bin_data[{ii}].bin_floor")
  drg.add_variable(f"test.bin_counter_arr_open.bin_data[{ii}].count")
for ii in range(3):
  drg.add_variable(f"test.bin_counter_interval.bin_data[{ii}].bin_floor")
  drg.add_variable(f"test.bin_counter_interval.bin_data[{ii}].count")
for ii in range(5):
  drg.add_variable(f"test.bin_counter_interval_open.bin_data[{ii}].bin_floor")
  drg.add_variable(f"test.bin_counter_interval_open.bin_data[{ii}].count")