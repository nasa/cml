# dimension = 9 for this test, as initialized in TestCorrelatedStateDispersion

dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

for ii in range(9) :
  for jj in range(ii+1) :
    dr_group.add_variable("simple_so.test.delta[%d][%d]" %(ii,jj) )

trick.stop(0)
