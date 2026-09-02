drg = trick.DRAscii("test_data")
drg.set_cycle(1)
drg.freq = trick.DR_Always
trick.add_data_record_group(drg, trick.DR_Buffer)

drg.add_variable("test.ellipsoid_intersection.intersection")
for ii in range(3):
  drg.add_variable(f"test.root1[{ii}]")
for ii in range(3):
  drg.add_variable(f"test.root2[{ii}]")
