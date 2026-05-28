# Based on RUN_06_spec_6_file, but the data load will be attempted with
# an invalid profile number.
# Binary read should fail.
exec(open( "RUN_06_spec_6_file/input.py").read())
luwinds.lookup_table_winds.wind_number = 12
