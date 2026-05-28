#   Covers check for perform_full_search in 
#       TableIndependentVariable::update()
#   Arrives at check for fraction < 0 in
#       TableIndependentVariable::generate_fraction()
#        must use debugger to set fraction < 0
#   Arrives at check for fraction > 1 in
#       TableIndependentVariable::generate_fraction()
#       must use debugger to set fraction > 1
code_coverage.tiv_binary_search();

trick.stop(0)