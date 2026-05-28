# Test activating the model with no preloaded data files and a bad
# filename specification.
# Should fail when model activates at t=2.
exec(open( "input_common.py").read())

luwinds.lookup_table_winds.unsubscribe()

luwinds.lookup_table_winds.drwpFileName = "Binaries/DRWP_w_comp_bad.bin"
luwinds.lookup_table_winds.wind_number = 2
luwinds.lookup_table_winds.include_vertical_component = True

trick.add_read(2,"luwinds.lookup_table_winds.subscribe()")
