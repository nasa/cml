dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always

for ib in range(3):
  for frame in ["composite_properties", "core_properties"]:
    dr_group.add_variable("so.body[%d].%s.mass" %(ib, frame))

    for ii in range(3) :
      dr_group.add_variable("so.body[%d].%s.position[%d]" %(ib, frame, ii))

    for ii in range(3) :
      for jj in range (3) :
        dr_group.add_variable("so.body[%d].%s.inertia[%d][%d]" %(ib, frame, ii, jj))

trick.add_data_record_group(dr_group, trick.DR_Buffer)
