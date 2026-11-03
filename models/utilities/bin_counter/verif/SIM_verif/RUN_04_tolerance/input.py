"""
Testing bin counter constructors and methods for a range of values.
"""

exec(open("Log_data/log_data_bin_tol.py").read())


test.framework.data_file_name = "Unit_test_data/data.txt"
test.framework.vars_file_name = "Unit_test_data/variables.txt"

test.bin_counter_vec.apply_tolerance(0.001)

trick.add_read(0.0, "test.bin_counter_arr_open.apply_tolerance(0.001)")

trick.stop(13)