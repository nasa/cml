#  These checks can not be reached under normal conditions.
#     One must use a debugger and set breakpoints to cover the
#     following sections of code
#  Arrives at check for independent variable data points <= 1 in 
#     GenericMultiInputTable::generate_base_values()
#     must use a debugger to set this value to <= 1
#  Arrives at size check for dwell in
#      GenericMultiInputTable::generate_base_values()
#      must use a debugger to set this size > 0.5 * number of interp points
#  Arrives at default case for switch on type of interpolation in
#      GenericMultiInputTable::generate_base_values()
#      must use a debugger to set value out of bounds
#  Arrives at check for output_ptrs_set in 
#      GenericMultiInputTable::precheck_output()
#      must use a debugger to set outpout_ptrs_set = False
#  Arrives at comparison between data_point_size and data_point_weight size
#      in GenericMultiInputTable::precheck_output()
#      must use a debugger to set unequal values
#  Arrives at precheck_output() call in
#      GenericMultiInputTable::generate_output()
#      must use a debugger to set this to false
code_coverage.gmit_generic_update()

trick.stop(0)