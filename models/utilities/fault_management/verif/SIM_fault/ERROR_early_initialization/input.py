exec(open('RUN_linear/input.py').read())

print ("\n"
"********************************************************\n"
"ERROR - Fault initialization before independent variable\n"
"********************************************************")
test_object.fault_function.initialize()
trick.stop(0)
