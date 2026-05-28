# Test model with no preloaded data. Just specify filename etc. and
# let the model pull it at activation.
# Should agree with RUN_02.
exec(open( "input_common.py").read())

luwinds.lookup_table_winds.drwpFileName = "Binaries/DRWP_w_comp.bin"
luwinds.lookup_table_winds.wind_number = 2
luwinds.lookup_table_winds.include_vertical_component = True
trick.stop(56)
