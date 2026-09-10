exec(open( "Log_data/msl_alt.py").read())

mslUnitTest.framework.data_file_name = "Unit_Test_Data/example_oceanic_coordinates.txt"
mslUnitTest.framework.vars_file_name = "Unit_Test_Data/variables.txt"

mslUnitTest.msl_alt.subscribe()

trick.stop(2)
