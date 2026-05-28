print("***** test pre-init execution ******")
test.tiv_error.update()


print("***** test double-loading of independent-data ******")
print("***** TIV:load_data(...) will fail, for       ******")
print("***** tiv_name and tiv_noname.                ******")
test.load_independent_data()

print("******* load tiv_error with a single data point   *******")
print("**** No class-specific output, provides check that   ****")
print("**** independent variable is interpreted as constant ****")
test.load_error_data()


trick.stop(0.1)
