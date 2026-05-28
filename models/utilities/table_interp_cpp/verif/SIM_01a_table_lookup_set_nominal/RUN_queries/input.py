# tests the queries available from Generic MultiInputTable and
# TableIndependentVariable

print("*******************************")
print("*** Prior to initialization ***")
print("*******************************")
print("GenericTable -- initialized:"), 
print(testing_model.test_so_defined.single_table.is_initialized())
print("GenericTable -- data_loaded:"),
print(testing_model.test_so_defined.single_table.is_data_loaded())

print("TIV -- is off-table-high:"),
print(testing_model.test_so_defined.indep0.is_off_table_back())
print("TIV -- is off-table-low:"),
print(testing_model.test_so_defined.indep0.is_off_table_front())

print('\n*******************************')
print("*** Post-initialization ***")
print("*******************************")
print('GenericTable -- initialized, loaded'),
trick.add_read(0, """
print(testing_model.test_so_defined.single_table.is_initialized()),
print(testing_model.test_so_defined.single_table.is_data_loaded())
""")
trick.stop(1)
