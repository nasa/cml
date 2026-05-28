# Test model with a preload of a 5-element and a 6-element data set,
# and no other specification. Should default to the first set loaded,
# so agree with RUN_01.
exec(open( "input_common.py").read())

luwinds.lookup_table_winds.load_DRWP_file("Binaries/DRWP_no_w_comp.bin", False, 1771)
luwinds.lookup_table_winds.load_DRWP_file("Binaries/DRWP_w_comp.bin",    True,  2)
