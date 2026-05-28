exec(open("Modified_data/env_setup.py").read())

# Insert specific initial conditions in the Unit_test files:
contact_obj.framework.data_file_name = "Unit_test_data/rate_data.txt"
contact_obj.framework.vars_file_name = "Unit_test_data/rate_variables.txt"

trick.add_read(7, '''
contact_obj.ref.composite_body.state.trans.position[0] = 0.3
contact_obj.ref.composite_body.state.trans.position[1] = 1.3
contact_obj.ref.composite_body.state.trans.position[2] = 2.3
contact_obj.ref.composite_body.state.rot.Q_parent_this.scalar = 0
contact_obj.ref.composite_body.state.rot.Q_parent_this.vector[0]= 0
contact_obj.ref.composite_body.state.rot.Q_parent_this.vector[1]= 0
contact_obj.ref.composite_body.state.rot.Q_parent_this.vector[2]= 1
''')


trick.stop(7.8)
