# This run attempts to call manager-initialize at multiple points during the
# configuration sequence.  At each point, the appropriate message should get
# dropped indicating what part of the configuration has not yet been completed.
# The configuration follows the pattern laid out in Modified_data/common_input.py.

print("***** TEST initialize before data")
test.manager.initialize()

# add a table
test.manager.add_table( test.generic_table)

print("***** TEST initialize before any TIV registered")
test.manager.initialize()

# add a TIV
test.manager.add_independent_variable( test.tiv_1)

print("***** TEST duplicate registry of TIV")
test.manager.add_independent_variable( test.tiv_1)

print("***** TEST initialize before any dependents registered")
test.manager.initialize()

# load dependent data
test.load_dependent_data()

print("***** TEST duplication of table")
test.manager.add_table( test.generic_table)

print("***** TEST initialize before TIV data loaded (2 errors)")
# Note - tiv has been added to manager, but it has no data.
#        so tiv initialization fails.
#        Hence, manager initialization also fails
test.manager.initialize()

# populate tiv, tiv_2, and tiv_3
test.load_independent_data()

print("***** TEST initialize before table has all TIV instances (2 errors)")
# Note - failure to initialize the table (because it has more data than inputs)
#        results in a failure to initialize the manager that manages it.
test.manager.initialize()

# Provide the other independents to the table
test.generic_table.add_independent( test.tiv_2)
test.generic_table.add_independent( test.tiv_3)

print("***** TEST initialize before manager has all TIV instances")
test.manager.initialize()

# Register the other independents with the manager
test.manager.add_independent_variable( test.tiv_2)
test.manager.add_independent_variable( test.tiv_3)


print("***** Redundant initialization error because table has already")
print("***** initialized successfully")
test.manager.initialize()

print("***** TEST duplicate initialize call")
test.manager.initialize()

print("***** TEST add independent post-initialization")
test.manager.add_independent_variable( test.tiv_1)

print("***** TEST add table post-initialization")
test.manager.add_table( test.generic_table)

trick.stop(0)
