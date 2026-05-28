# Tests the sanity checks on the loading of data before the table gets
# initialized.  Note - in this test, we test initialization with:
# - neither independent nor dependent data
# - dependent data but no independent data
# We do not test with independent data but no dependent data; the dependent
# data check comes first, so without dependent data, the presence (or absence)
# of independent data is irrelevant.

# Model must be initialized before running.  Check.
print("***** INPUT TEST update before initialize")
test.generic_table.update()

# Data must be loaded before initialization.  Check.
print("***** INPUT TEST initialize before data")
test.generic_table.initialize()

# Populate dependent data
test.load_dependent_data()

# Now try to initialize again.
# Don't have independents yet, this should fail
# because dependent data is expecting 3 independents.
print("***** INPUT TEST initialize without independents")
test.generic_table.initialize()

# Populate independent data
test.load_independent_data()
test.generic_table.add_independent( test.tiv_2)
test.generic_table.add_independent( test.tiv_3)
test.generic_table.initialize()

# test late-addition failures.
test.add_table_vars_post_init()

# test reassignment of output
print("***** TEST reassign output pointers")
test.reassign_output()

trick.stop(0)
