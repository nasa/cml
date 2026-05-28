# Fail case:
# attempt to compute the interpolation before the node table has data
# NOTE -- use lagged_data_noname to confirm the default name ("LaggedAtmos")
test.lagged_data_noname.compute(1.0)
