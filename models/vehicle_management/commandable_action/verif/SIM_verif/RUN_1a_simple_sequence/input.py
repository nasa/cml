exec(open("Log_data/log_data.py").read())

# Insert specific initial conditions in the Unit_test files:
test.framework.data_file_name = "Unit_test_data/data1.txt"
test.framework.vars_file_name = "Unit_test_data/variables.txt"

# Set the termination time.
# The termination time should be the number of tests that you will perform.
# In this case, the model will be tested under 4 sets of initial conditions.
trick.stop(9)
