# Specify which framework to use:
test.initialize_with_R3 = True
test.framework_provides_R3 = True

exec( open("Modified_data/env_setup.py").read())
exec( open("Log_data/log_data.py").read())
trick.stop(5)
