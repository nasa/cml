# This test processes data from an external file.
# It creates a new instance of TableIndependentVariable (TIV)
# RUN_01_internal_data and RUN_02_external_data outputs should match
exec(open("Log_data/log_data.py").read())
test.process_external_data()
test.initialize()
trick.stop(5)
