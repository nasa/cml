# Tests the sanity check that the user has not added more than 1 TIV
print("*** Test adding 2 TableIndependentVariable instances to manager's list")
test.use_TIV_1()
test.add_TIV_2()
test.process_internal_data()

trick.stop(0)
