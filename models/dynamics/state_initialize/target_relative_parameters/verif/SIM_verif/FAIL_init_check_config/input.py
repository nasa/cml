# Copy/paste MOST of env_setup.py here
# Tedious, but notable exception to copy/paste are the setting
# of the two planet instances
# Will lead to a NULL check failure in initialize_check_config()
import math

test.planet.pfix.state.rot.ang_vel_this = [0,0,1]
test.planet.r_eq = 10E3
test.planet.r_pol = 9.9995E3
test.planet.e_ellipsoid = 0.01

test.tgt_alt = 0
test.tgt_lat = 0
test.tgt_lon = math.radians(90)
test.K_theta = 0.9

test.ref_alt = 10000
test.ref_lat = math.radians(30)
test.ref_lon = math.radians(-100)

test.ref_position = [8000, 5000, 3350]
test.ref_position = [0, 0, 10000]

if (test.framework_provides_R3):
  test.framework.data_file_name = "Unit_test_data/data_R3.txt"
  test.framework.vars_file_name = "Unit_test_data/variables_R3.txt"
else:
  test.framework.data_file_name = "Unit_test_data/data_params.txt"
  test.framework.vars_file_name = "Unit_test_data/variables_params.txt"

test.framework_provides_R3 = False
test.framework.enabled = False
test.params_in.Lambda = 1.1
test.params_in.theta_Rng = 1.0
test.params_in.Vmag = 1
trick.stop(0)
