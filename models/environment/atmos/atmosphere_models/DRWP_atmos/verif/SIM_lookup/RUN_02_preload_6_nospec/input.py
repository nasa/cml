# Test model with a preload of a 6-element data set and no other
# specification.
exec(open( "input_common.py").read())

# DRWP binary file info
#  filename
#  include vertical wind component? True or False
#  wind number
luwinds.lookup_table_winds.load_DRWP_file("Binaries/DRWP_w_comp.bin", True, 2)
trick.stop(56)
