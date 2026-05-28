# Test model with a preload of a 5-element and a 6-element data set,
# and then specify the profile by index.
# Should pick out the second set loaded, so agree with RUN_02.
exec(open( "RUN_03_preload_56_nospec/input.py").read())

luwinds.lookup_table_winds.change_datafile_index(1)
trick.stop(56)
