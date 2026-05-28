# Tests the detection of a NULL independent variable.
# independent_var is set in default data, so is "unset" here
print("*** TEST data-processing with undefined independent ***")
test.manager.independent_var = None
test.process_internal_data()

trick.stop(0)
