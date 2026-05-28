# Test trying to load a DRWP file with a bad filename.
exec(open( "input_common.py").read())

# wind profile
# Set DRWP filename to nonexistent path
luwinds.lookup_table_winds.load_DRWP_file("Binaries/this/path/does/not/exist.bin", False, 1771)
