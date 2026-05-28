# The FAIL_bad_binary set of runs use corrupt data files found in
# Binaries/corrupt_binaries to test each scheduled read of the binary file.
# This one tests the effect of having an invalid specification for the array
# of altitude-based calibration points, in this case, the array is truncated and
# incomplete.
exec(open( "input_common.py").read())
luwinds.lookup_table_winds.load_DRWP_file("Binaries/corrupt_binaries/short_altitude_array.bin", True, 11)

