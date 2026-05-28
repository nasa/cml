drg1 = trick.sim_services.DRAscii("test_data")
drg1.set_cycle(1.0)
drg1.freq = trick.sim_services.DR_Always
trick.add_data_record_group(drg1, trick.DR_Buffer)

drg1.add_variable( "test.sample.buffer_size")
drg1.add_variable( "test.sample.buffer.buffer_ix")
for ii in range(10):
  drg1.add_variable( "test.sample.buffered_tags[%d]"%ii)
  drg1.add_variable( "test.sample.buffered_instances[%d].x"%ii)
  drg1.add_variable( "test.sample.buffered_instances[%d].i"%ii)


drg2 = trick.sim_services.DRAscii("lookup_data")
drg2.set_cycle(1.0)
drg2.freq = trick.sim_services.DR_Always
trick.add_data_record_group(drg2, trick.DR_Buffer)

drg2.add_variable( "test.sample.lookup_tag")
drg2.add_variable( "test.sample.lookup_x")
