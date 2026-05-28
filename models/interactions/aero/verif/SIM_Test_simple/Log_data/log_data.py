dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always

dr_group.add_variable("aero.atmos_rel.dynamic_pressure")
dr_group.add_variable("aero.interface.executive.drag_mag")
for ii in range(0,3):
  dr_group.add_variable("aero.atmos_rel.free_stream_vel_unit[%d]" %ii);
for ii in range(0,3):
  dr_group.add_variable("aero.interface.output.force[%d]" %ii);
trick.add_data_record_group(dr_group, trick.DR_Buffer)
