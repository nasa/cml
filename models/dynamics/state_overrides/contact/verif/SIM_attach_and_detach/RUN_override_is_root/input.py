exec(open("Modified_data/env_setup.py").read())

# Override variables
contact_obj.framework.data_file_name = "Unit_test_data/data_is_root.txt"
contact_obj.framework.vars_file_name = "Unit_test_data/variables_is_root.txt"

# Separate override body from its parent body ("root-body")
trick.add_read(1.5, "contact_obj.override.detach()")

trick.add_read(5,'''
contact_obj.ref.composite_body.state.trans.position[0] = -0.5
contact_obj.ref.composite_body.state.trans.position[1] = -1.0
''')
trick.add_read(6,'''
contact_obj.ref.composite_body.state.trans.position[0] = 0.5
contact_obj.ref.composite_body.state.trans.position[1] = 1.0
''')
