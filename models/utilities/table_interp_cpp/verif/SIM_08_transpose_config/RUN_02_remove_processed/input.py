# This test processes data from the compiled code.
# It creates a new instance of TableIndependentVariable (TIV)
test.process_data()
test.initialize()

print("***remove config after data processed -- produces warning***")
test.manager.remove_config( test.config1)

trick.stop(0)
