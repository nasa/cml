# Switch between preloaded data profiles mid-sim by index.
# Also tests the reset of the warning_issued flag, get the altitude domain
#  warning on the first and second profile.
# For t between:
#   [0,9] data should match RUN_02
#   [10,19] new data
#   [20,29 data should match RUN_01
#   [30,40] data should match RUN_02
exec(open( "input_common.py").read())

# the default data set index = 0
# First wind profile - file name, vertical component(0/1), wind profile
luwinds.lookup_table_winds.load_DRWP_file("Binaries/DRWP_w_comp.bin", True, 2)
# Second wind profile
luwinds.lookup_table_winds.load_DRWP_file("Binaries/DRWP_w_comp.bin", True, 3)
# Third wind profile
luwinds.lookup_table_winds.load_DRWP_file("Binaries/DRWP_no_w_comp.bin", False, 1771)

trick.add_read(10, "luwinds.lookup_table_winds.change_datafile_index(1)")
trick.add_read(20, "luwinds.lookup_table_winds.change_datafile_index(2)")
trick.add_read(30, "luwinds.lookup_table_winds.change_datafile_index(0)")

trick.stop(39)
