exec(open("Modified_data/env_setup.py").read())

# Insert specific initial conditions in the Unit_test files:
contact_obj.framework.data_file_name = "Unit_test_data/data.txt"
contact_obj.framework.vars_file_name = "Unit_test_data/variables.txt"

trick.stop(10.8)
