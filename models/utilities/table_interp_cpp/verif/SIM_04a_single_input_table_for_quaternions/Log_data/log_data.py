dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always
dr_group.add_variable("test.independent")
for ii in range(4):
  dr_group.add_variable("test.output_quaternion_simple[%d]" %ii)
for ii in range(4):
  dr_group.add_variable("test.output_quaternion_default[%d]" %ii)
for ii in range(4):
  dr_group.add_variable("test.output_quaternion_array[%d]" %ii)
for ii in range(4):
  dr_group.add_variable("test.output_quaternion_linear[%d]" %ii)

dr_group.add_variable("test.output_quaternion_Quaternion.scalar")
for ii in range(3):
  dr_group.add_variable("test.output_quaternion_Quaternion.vector[%d]" %ii)

trick.add_data_record_group(dr_group, trick.DR_Buffer)
