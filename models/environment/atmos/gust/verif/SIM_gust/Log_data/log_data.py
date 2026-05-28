dr_group = trick.DRAscii("verif")
dr_group.set_cycle(0.5)
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable(   "gust_unit.gust.magnitude")
for ii in range(3) :
  dr_group.add_variable("gust_unit.gust.gust_vector[%d]"%ii)

#def
#dr_group.add_variable(   "gust_unit.time")
#dr_group.add_variable(   "gust_unit.gust.maximum_magnitude")
#dr_group.add_variable(   "gust_unit.gust.period")
#dr_group.add_variable(   "gust_unit.gust.phase_angle")
#for ii in range(0,3) :
#  dr_group.add_variable("gust_unit.gust.direction[%d]" %ii)
