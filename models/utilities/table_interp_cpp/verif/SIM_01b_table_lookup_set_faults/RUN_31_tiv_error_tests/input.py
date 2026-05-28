print("***TEST: calling TableIndependentVariable.load_data with a NULL array***")
test.test_tiv_null();

print("***TEST: calling TableIndependentVariable.load_data with array size 0***")
test.test_zero_length();

print("***TEST: calling TableIndependentVariable.load_data with non-monotonic data***")
test.test_non_monotonic();

print("***TEST: calling TableIndependentVariable.load_data with duplicate values***")
test.test_duplicate_values();

trick.stop(0)
