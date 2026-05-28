# Tests the application of the altitude bias
exec(open( "RUN_01_preload_5_nospec/input.py").read())

# add an altitude bias
luwinds.lookup_table_winds.alt_bias = 4000.0
trick.stop(5)
