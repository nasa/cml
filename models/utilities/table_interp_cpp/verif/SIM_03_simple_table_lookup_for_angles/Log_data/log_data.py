dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always
dr_group.add_variable("test.independent")
for ii in range(2):
  dr_group.add_variable(f"test.dependent_degrees[{ii}]")
for ii in range(2):
  dr_group.add_variable(f"test.dependent_radians[{ii}]")

trick.add_data_record_group(dr_group, trick.DR_Buffer)
