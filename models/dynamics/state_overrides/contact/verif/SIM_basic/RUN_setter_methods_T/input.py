exec(open("Modified_data/env_setup.py").read())

# Test out the setter methods
contact_obj.contact.set_contact_normal([1.0, 0.2, 0.0])
contact_obj.contact.set_contact_position([2.0, 0.0, 0.0])
contact_obj.contact.set_contact_orientation([[-1,0,0],[0,1,0],[0,0,-1]])
contact_obj.contact.set_deactivation_threshold(1.5)

# Trigger the errors in the setter methods
trick.add_read(5.0, '''
contact_obj.contact.set_deactivation_threshold(0.5)
contact_obj.contact.set_contact_orientation([[-1,0,0],[0,1,0],[0,0,-1]])
contact_obj.contact.set_contact_normal([0.0, 1.0, 0.0])
''')

contact_obj.framework.data_file_name = "Unit_test_data/setter_data.txt"
contact_obj.framework.vars_file_name = "Unit_test_data/variables.txt"

trick.stop(6.8)
