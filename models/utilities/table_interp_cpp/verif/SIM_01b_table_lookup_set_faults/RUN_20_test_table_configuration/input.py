print("**************************************************************")
print("*** executing populate_table_with_invalid_data_dependent() ***")
print("**************************************************************\n")
test.populate_table_with_invalid_data_dependent()
print("**************************************************************")
print("***** end of populate_table_with_invalid_data_dependent() ****")
print("**************************************************************\n")

#Add tiv_2, succeeds
test.generic_table.add_independent( test.tiv_2);

# Try replacing it with tiv_1
# Note - need 3 arguments to break overload ambiguity.
print("***TEST: override independent ***")
test.generic_table.add_independent( test.tiv_1, 0, 0);

print("**** TEST duplicate addition of independent to table")
test.generic_table.add_independent( test.tiv_1)

# Add tiv_3 to slot-2, leaving slot-1 NULL.
test.generic_table.add_independent( test.tiv_3, 2, 0)

# Now initialize -- will fault at the first entry in the TIV array because it
# has no data.
print("***** TEST initialize without independent-data")
test.generic_table.initialize()

#  Put inconsistent data into the TIV instance.
test.populate_TIV_with_inconsistent_data()

# Now initialize -- will fault at the first entry in the TIV array because it
# has invalid data.
print("***** TEST initialize with inconsistent data array sizes")
test.generic_table.initialize()

# Run checks on resetting independent data
print("***TEST: attempt to overwrite independent data. ***")
test.reset_TIV_data()

# Now really overwrite the data
test.tiv_1.clear_data()
test.reset_TIV_data()

# Try to initialize with invalid TIV array, this should fault at the second
# slot (slot-1), which is still empty.
print("*** TEST initialize with one of the independents unassigned ***")
test.generic_table.initialize()

# Fix that.
test.generic_table.add_independent( test.tiv_2,1,0)

print("*** TEST initialize with one of the independents with no data ***")
test.generic_table.initialize()

trick.stop(0)
