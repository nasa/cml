dr_group = trick.sim_services.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.sim_services.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable( "test.sample.buffer_size")
dr_group.add_variable( "test.sample.buffer.buffer_ix")
for ii in range(10):
  dr_group.add_variable( "test.sample.output[%d].x"%ii)
  dr_group.add_variable( "test.sample.output[%d].i"%ii)
