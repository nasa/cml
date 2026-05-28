# This test processes data from the compiled code.
# It creates a new instance of TableIndependentVariable (TIV)
exec(open("Log_data/log_data.py").read())
test.process_internal_data()
test.initialize()
trick.stop(5)
