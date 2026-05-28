# Test model with a preload of a 5-element and a 6-element data set,
# and then specify the filename and profile.
# Should pick out the second set loaded, so agree with RUN_02.
exec(open( "RUN_03_preload_56_nospec/input.py").read())

luwinds.lookup_table_winds.drwpFileName = "Binaries/DRWP_w_comp.bin"
luwinds.lookup_table_winds.wind_number = 2
luwinds.lookup_table_winds.include_vertical_component = True
trick.stop(56)
