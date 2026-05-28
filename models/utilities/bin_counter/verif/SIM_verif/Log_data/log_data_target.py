"""
Logging data for Target Counter type
"""

drg = trick.sim_services.DRAscii("test_data")
drg.set_cycle(1.0)
drg.freq = trick.sim_services.DR_Always
trick.add_data_record_group(drg, trick.DR_Buffer)

for ii in range(4):
    drg.add_variable(f"test.tgt_counter_vec.target_data[{ii}].value")
    drg.add_variable(f"test.tgt_counter_vec.target_data[{ii}].count")
for ii in range(4):
    drg.add_variable(f"test.tgt_counter_arr.target_data[{ii}].value")
    drg.add_variable(f"test.tgt_counter_arr.target_data[{ii}].count")
for ii in range(4):
    drg.add_variable(f"test.tgt_counter_interval.target_data[{ii}].value")
    drg.add_variable(f"test.tgt_counter_interval.target_data[{ii}].count")
