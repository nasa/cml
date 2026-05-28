# Error case:
# attempt to set the minimum delta to an invalid value
test.lagged_data_noname.set_min_delta_altitude(0.0)
test.lagged_data_noname.set_min_delta_altitude(-1.0)
test.framework.data_file_name = "Unit_test_data/data.txt"
test.framework.vars_file_name = "Unit_test_data/variables.txt"
trick.stop(0)
