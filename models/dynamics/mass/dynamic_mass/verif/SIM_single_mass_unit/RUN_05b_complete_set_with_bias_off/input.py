exec(open("RUN_05a_complete_set_with_bias/input.py").read())
# Check that turning the flag off in the input file blocks the dispersions.
# Data should match that from the non-dispersed run, RUN_04...
mass_test.tank.dynamic_properties.mass_dispersion_flag = False
