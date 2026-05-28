## Pre-import tedium
#import os;
#import sys;
#run_directory = os.path.dirname(os.path.realpath(__file__))
#sys.path.append(f"{os.environ['CML_HOME']}/..")  # To import cml utils
#
## Now we can import things
#import trick
#import cml.utility.unit_testing as unit_testing
#from cml.utility.logging import CmlLogAscii
#
## Configure the run
#test_interval = 1.0
#trick.set_output_dir(f"{run_directory}/output")
#cml_math.which_testing = cml_math.ARR_OPS
#
## Configure logging
#dr_group = CmlLogAscii("profile_arr_ops", cycle=test_interval)
#dr_group.add("cml_math.arr_ops_data.vec3")
#dr_group.add("cml_math.arr_ops_data.vec3_c")
#dr_group.add("cml_math.arr_ops_data.scalar")
#
## Configure testing
#path_to_tests = f"{run_directory}/tests.jsonc"
#stop_time = unit_testing.add_tests_from_file(path_to_tests, test_interval)

# End sim after testing is done
test.in_vec_3 = [3.1, 3.2, 3.3]
test.in_vec_4 = [4.1, 4.2, 4.3, 4.4]
trick.stop(0)
