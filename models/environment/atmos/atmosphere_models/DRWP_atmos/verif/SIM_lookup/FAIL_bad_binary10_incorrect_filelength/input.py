# The FAIL_bad_binary set of runs use corrupt data files found in
# Binaries/corrupt_binaries to test each scheduled read of the binary file.
# This one tests the effect of having an data block with a size that is
# inconsistent with expectation.
exec(open( "input_common.py").read())
luwinds.lookup_table_winds.load_DRWP_file("Binaries/corrupt_binaries/incorrect_filelength.bin", True, 11)
