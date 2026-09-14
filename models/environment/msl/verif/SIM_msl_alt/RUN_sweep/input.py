import math
exec(open( "Log_data/msl_alt.py").read())
mslUnitTest.lat_lon_sweep(10,10)

mslUnitTest.msl_alt.subscribe()
trick.stop(19*37)
