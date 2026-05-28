# This run executes the test values provided by NGA

#      lon       lat       height
#        241,     37,     -26.151
#       -119,     37,     -26.151
# 242.983333,     36,     -29.171
#          0,     90,      14.899
# 359.983333,    -90,     -30.150
#          0,    -90,     -30.150

# The locations that hit the calibrated points match
# The location at lon = 242.983333 has a height difference of about 7cm due
# to a differenc in interpolation (this uses linear versus a 6-point
# spline, and has fewer data points)

exec( open("Log_data/log_data.py").read())

test.egm.subscribe()

test.framework.data_file_name = "Unit_test_data/data.txt"
test.framework.vars_file_name = "Unit_test_data/variables.txt"

trick.stop(5)
