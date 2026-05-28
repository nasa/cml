# Based on RUN_06_spec_6_file, but the data load will be attempted with
# an assumption that the data file does not include a vertical winds component.
# Binary read should fail because the reader will advance to read the next
# profile number while it is still in the previous profile data so the next
# profile number will not match with expectation.
exec(open( "RUN_06_spec_6_file/input.py").read())
luwinds.lookup_table_winds.include_vertical_component = False
