# This test is used to make sure the check at generic_multi_input_table.cc:312 works
# as intended. But before that check is hit there is one that is tripped before it at
# table_independent_variable.cc:146. Where a warning message appears telling the user
# that for the independent variable loaded there is only one data pt, and that there
# isn't anything wrong but that there's nothing to lookup and the output is constant.

# Then once this warning is done the check at generic_multi_input_table.cc:312 that
# this test is for is tripped. This check is to make sure that the number of
# independent variables is consistent with the number of dependent variables. If they're
# not like in this case then an error message will printo out to the screen tellign the
# user that the independent variables and dependent variables don't share the same size
# and will print out how many data pts there are of each.

test.high_index()
test.tiv_4.update()

# This next check is at table_independent_variable.cc:440 where the model checks to see
# if there was already a name set for the independent variable. And if there was a name
# already set then the model warns the user of it and that the name is being changed to
# the new one the user wanted to set it to.
test.name()

# The next error message that appears is at generic_multi_input_table.cc:420 when the
# update() function for the model is called. Since the model failed the check at 312
# in the initialization function the model was never initialized properly. Which causes
# it to fail at 420 when it checks to see if the model was initialized correctly or not.
# Since it wasn't an error message prints out to the screen telling the user that the
# model wasn't initialized yet and that it can't proceed with interpolating or
# integrating any values or looking up any values.

trick.stop(0)
