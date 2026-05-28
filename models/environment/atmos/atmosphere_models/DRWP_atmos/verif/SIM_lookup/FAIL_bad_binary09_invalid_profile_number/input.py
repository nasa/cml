# The FAIL_bad_binary set of runs use corrupt data files found in
# Binaries/corrupt_binaries to test each scheduled read of the binary file.
# This one tests the effect of having an invalid specification for the profile
# number at the start of the profile. This should match with the respective
# value in the profile-array data.
exec(open( "input_common.py").read())
luwinds.lookup_table_winds.load_DRWP_file("Binaries/corrupt_binaries/invalid_profile_number.bin", True, 11)
