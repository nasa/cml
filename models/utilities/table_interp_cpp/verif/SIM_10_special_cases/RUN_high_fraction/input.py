# This test is used to specifically hit the check at generic_multi_input_table.cc:167
# where the values for dim_list were inputted at a higher value than the indp_data
# that would be passed through load_data() in table_independent_variable.cc since the
# values from these two vectors do not match up the check at 165 doesn't pass and an
# error message prints out to the screen telling the user the dependent variables
# and the output values do not match up.

# Then after the intended check a few more appear. The first is a check on the independent
# variables to see how many data pts there are. If there is only one data pt then a warning
# message prints out to the user at table_independent_variable.cc:146 telling them that
# there is only one data pt, and that this isn't necessarily wrong just that there isn't
# anything to lookup and output values will be constant.

# The next check is at generic_multi_input_table.cc:292 where the model fails to initialize
# correctly because it failed to load the data correctly at 165. Once the check is failed
# an error message prints out to the screen telling the user that they haven't populated the
# table with data yet and so can't initialize the model properly.

# Then since the initialization of the model failed at 292 at there is another check that fails
# at generic_multi_input_table.cc:420 when the update() function is called for the model.
# Since the model wasn't initialized properly it fails the check at 420 and is not able to update
# properly either as an error message prints out to the user telling them that the model hasn't
# been initialized and can't proceed with interpolation or variable lookup.

test.fraction()
test.tiv_5.update()

trick.stop(0)
