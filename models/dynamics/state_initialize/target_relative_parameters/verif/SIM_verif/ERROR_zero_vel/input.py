exec( open("RUN_R3/input.py").read())
# Set input velocity to zero to elicit errors
test.framework.data_file_name = "Unit_test_data/data_R3_zero_velocity.txt"
trick.stop(1)
