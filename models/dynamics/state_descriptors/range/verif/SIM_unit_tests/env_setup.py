test.earth.name = "Earth"
test.earth.r_eq = 6378137.0
test.earth.e_ellip_sq = 0.006694379990141
test.gravity_source.name = "Earth"

if test.framework.enabled:
  test.framework.data_file_name = "Unit_test_data/data.txt"
  test.framework.vars_file_name = "Unit_test_data/variables.txt"

# Set the termination time.  The termination time should be the number of tests
# that you will perform minus one (because the first test is at t = 0).
# Remember the last line of csv file not properly displayed in Trick DP plotting
# tool (known issue)
  f = open(test.framework.data_file_name, "r")
  num_tests = len(f.readlines())
  print("Number of tests: " + str(num_tests))
  trick.stop(num_tests - 1)

# FOR DEBUGGING ONLY
#trick.real_time_enable()
#trick.sim_control_panel_set_enabled(True)
