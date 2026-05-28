# The FAIL_bad_binary set of runs use corrupt data files found in
# Binaries/corrupt_binaries to test each scheduled read of the binary file.
# This one tests the effect of having an invalid specification for the profile
# number at the start of the profile, in this case the file is truncated and
# this data point does not exist.
exec(open( "input_common.py").read())
luwinds.lookup_table_winds.load_DRWP_file("Binaries/corrupt_binaries/no_profile_number.bin", True, 11)

