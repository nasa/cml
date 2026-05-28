# This test case produces a sweep of the entire globe in 10 degree increments.
# A similar test case is run in the MSL model, then the MSL altitudes can be
# compared against the output of this model to ascertain how closely the two
# models match.
# Note that the msl-altitude is the definition of the altitude of the ellipsoid
# above MSL, while this model provides the altitude of the geoid
# above the ellipsoid, so they are arithmetic inverses.

exec( open("Log_data/log_data.py").read())
test.egm.subscribe()
test.lat_lon_sweep(10,10)
trick.stop(19*37)
