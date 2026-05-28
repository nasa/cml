# Test used to enable the full search option for the model and see if it works
# as it's intended to. Which would be done at table_independent_variable.cc:260
# where the "perform_full_search" flag is used.

# However, as with the other runs this one has some errors that will appear when
# running the input file. The first one to appear is for the usual single
# indpendent variable warning at table_independent_variable.cc:146 that tells
# the user that the data table was only loaded with one independent variable and
# that nothing is necessarily wrong but is warnign the user that there's nothing
# to lookup and output value is constant.

# Then next is a check at generic_multi_input_table.cc:376 where the model gives
# an additional warning to the user that the independent variable has only 1 data
# value and that there is nothing to interpolate or lookup, so it is being removed
# from the set of independents.

exec(open("RUN_1_2/input.py").read())

test.tiv_1.perform_full_search = True
test.tiv_2.perform_full_search = True
test.tiv_3.perform_full_search = True
