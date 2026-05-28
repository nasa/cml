import math

test.K_theta = 0.9

test.ref_position = [0, 0, 10]

# Make sure that both sets get the same target position.
test.tr_params_to_R3.target_point = [10.0, 5.0, 1.0]
test.tr_R3_to_params.target_point = [10.0, 5.0, 1.0]

test.tr_params_to_R3.omega_planet[2] = 1.0
test.tr_R3_to_params.omega_planet[2] = 1.0


if (test.framework_provides_R3):
  test.framework.data_file_name = "Unit_test_data/data_R3.txt"
  test.framework.vars_file_name = "Unit_test_data/variables_R3.txt"
else:
  test.framework.data_file_name = "Unit_test_data/data_params.txt"
  test.framework.vars_file_name = "Unit_test_data/variables_params.txt"
