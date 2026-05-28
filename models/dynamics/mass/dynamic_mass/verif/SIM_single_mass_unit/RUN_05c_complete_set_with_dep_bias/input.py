exec(open("RUN_05d_complete_set_with_bias_dep_and_indep/input.py").read())
# Check that turning an individual flag off in the input file blocks the
# dispersion of that set.
# Data should match in the decimals with data from the non-dispersed
#  run, RUN_04...
mass_test.tank.dynamic_properties.interpolation.tab_mass_flag = False
