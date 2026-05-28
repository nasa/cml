# PURPOSE:
#    (Log RCS generic test data.)
#
# PROGRAMMERS:   
#    (((Edward McCants) (CSDL) (Oct 2014) (ANTARES)))

dr_group = trick.DRAscii("rcs_generic")
dr_group.thisown = 0
dr_group.set_cycle(0.025)
dr_group.freq = trick.DR_Always

for ii in range(0,3) :
  dr_group.add_variable("test.rcs.force[%d]" %ii)
  dr_group.add_variable("test.rcs.torque[%d]" %ii)

  for jj in range(1,4) :
    dr_group.add_variable("test.rcs.jet_0%d.force[%d]" %(jj, ii))
    dr_group.add_variable("test.rcs.jet_0%d.torque[%d]" %(jj, ii))

dr_group.add_variable("test.rcs.rcs_pod_1.sum_consumption")
dr_group.add_variable("test.rcs.rcs_pod_2.sum_consumption")


trick.add_data_record_group(dr_group, trick.DR_Buffer)
