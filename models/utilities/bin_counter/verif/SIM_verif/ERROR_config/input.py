"""
Running test_warnings function defined to go over each warning in all the counters.
"""

test.framework.data_file_name = "Unit_test_data/data.txt"
test.framework.vars_file_name = "Unit_test_data/variables.txt"

trick.add_read(0, "test.test_warnings()")

trick.stop(0)
