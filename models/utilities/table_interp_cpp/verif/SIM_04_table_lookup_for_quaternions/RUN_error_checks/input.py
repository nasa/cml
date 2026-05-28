# This test is purely for code coverage and not for test data.

# Should see this error:
# Error Initialization error:
#  at single_input_table_for_quaternions.cc line 76:
# This type of table can only populate a quaternion but there are 3
# outputs specified.  There must be 4 outputs.
print("\n******** Test initializing a quaternion model with only 3 outputs and not 4 specified *****\n")
test.interpolation_table_error.initialize()

# These should work, putting them here because there is nowhere better
test.interpolation_table_array.set_epsilon_interp(0.00005)
test.interpolation_table_array.set_epsilon_delta(0.00005)

# Error Invalid optional argument.
#  at quaternion_spherical_interpolator.cc line 120:
# The epsilon value has a minimum of 1E-15.Resetting to minimum.
print("\n******** Test setting the epsilon value to something lower than the minimum *****\n")
test.interpolation_table_array.set_epsilon_delta(1.0E-17)

trick.stop(0.1)
