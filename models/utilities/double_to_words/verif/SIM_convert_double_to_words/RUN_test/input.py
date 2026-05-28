exec(open("Log_data/log_data.py").read())

# Insert specific initial conditions in the Unit_test files:
convert_double_words.framework.data_file_name = "Unit_test_data/data_test.txt"
convert_double_words.framework.vars_file_name = "Unit_test_data/variables.txt"

trick.stop(7)
