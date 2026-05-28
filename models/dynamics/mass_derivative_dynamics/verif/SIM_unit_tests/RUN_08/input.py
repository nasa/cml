# This run tests the effect of releasing mass at a variable rate along
# the x-axis while it is spinning about the z-axis.  Consequential torque
# should be variable and about the z-axis only.
exec(open( "RUN_01/input.py").read())
test.framework.data_file_name = "Unit_test_data/RUN_08.txt"
trick.stop(9)
