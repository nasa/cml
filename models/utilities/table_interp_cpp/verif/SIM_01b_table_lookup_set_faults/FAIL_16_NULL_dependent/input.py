# This test is to hit the check at table_manager.cc:380 where the variable_ptr
# going into verify_and_add_dependent_variable() is not a NULL. If it is it 
# fails the sim and prints out the failure message.
print("***TEST: Loads a NULL into the dependent variable table ***\n")
test.create_table_with_NULL()
exec(open("Modified_data/common_input.py").read())
trick.stop(0)
