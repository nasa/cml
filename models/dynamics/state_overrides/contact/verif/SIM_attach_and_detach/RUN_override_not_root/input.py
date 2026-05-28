exec(open("Modified_data/env_setup.py").read())

# Override specific variables
contact_obj.framework.data_file_name = "Unit_test_data/data_not_root.txt"
contact_obj.framework.vars_file_name = "Unit_test_data/variables_not_root.txt"

# Separate "root-body" from its parent ("reference-body")
trick.add_read(1.5, "contact_obj.root.detach()")
