# This test matches with RUN_01_internal_data except that it 
# uses the pre-instantiated TableIndependentVariable (TIV)
# The output data should be unaffected.
test.use_TIV_1()
exec(open("RUN_01_internal_data/input.py").read())
trick.stop(5)
