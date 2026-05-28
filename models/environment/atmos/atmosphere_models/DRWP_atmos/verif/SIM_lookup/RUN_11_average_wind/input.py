# Tests the calculation of the average winds across different domains.
# Also tests the blocking of the altitude warnings.
exec(open( "input_common.py").read())
for ii in range(3):
  drg.add_variable("luwinds.lookup_table_winds.average_wind[{0}]".format(ii))
#exec(open( "Log_data/log_average_wind.py").read())
trick.CMLMessage.set_publish_level( trick.CMLMessage.Inform)
luwinds.lookup_table_winds.subscribe()
luwinds.lookup_table_winds.load_DRWP_file("Binaries/DRWP_w_comp.bin", True, 2)
luwinds.lookup_table_winds.load_DRWP_file("Binaries/DRWP_no_w_comp.bin", False, 1771)
luwinds.lookup_table_winds.block_warnings = True

trick.add_read(1,"luwinds.lookup_table_winds.compute_average_wind()")
trick.add_read(2,"luwinds.lookup_table_winds.compute_average_wind(0)")
trick.add_read(3,"luwinds.lookup_table_winds.compute_average_wind(0,1000)")
trick.add_read(4,"luwinds.lookup_table_winds.compute_average_wind(0,10000)")
trick.add_read(5,"luwinds.lookup_table_winds.compute_average_wind(1)")
trick.add_read(6,"luwinds.lookup_table_winds.compute_average_wind(1,0,1000)")
trick.add_read(7,"luwinds.lookup_table_winds.compute_average_wind(1,0,10000)")
trick.add_read(8,"luwinds.lookup_table_winds.compute_average_wind(0,0.1)")
trick.add_read(9,"luwinds.lookup_table_winds.compute_average_wind(5,0,1000)")

trick.stop(9)
