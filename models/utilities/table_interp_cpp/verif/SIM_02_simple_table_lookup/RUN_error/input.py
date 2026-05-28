test.configure_table_1_indep_val()

print("\n*************** no data whatsoever ******************\n")
test.table_error.initialize()

print("\n*************** test loading NULL values ******************\n")
test.load_NULL_tests()


print("\n******** test loading independent variable with duplicate values*****\n")
test.load_duplicate_independent()

# Even though loading independent data is invalid (and unusable), there is
# an independent variable now (waiting for data).
print("\n******** test init with independent variable but no data *****\n")
test.table_error.initialize()

# load data onto the independent variable.  will get a warning about
# overriding previous failed load
print("\n******** overwrite previously failed load **********\n")
test.load_independent_on_error()

print("\n******** test init without data table **********\n")
test.table_error.initialize()

# create a data-table.  Note that - unlike the independent data - it is
# not possible to try loading data and have it fail but leave the 
# data table behind.  So we have to somewhat articifially create a new
# data table.
test.create_dummy_table()

print("\n******** test init without table-data **********\n")
test.table_error.initialize()


print("\n******** test checks for identifying badly sized table-data ********\n")
test.load_bad_size()


trick.stop(0.1)
