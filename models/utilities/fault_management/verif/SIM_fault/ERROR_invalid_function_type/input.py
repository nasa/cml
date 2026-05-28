exec(open('RUN_linear/input.py').read())

print ("\n"
"********************************************************\n"
"ERROR - invalid function-type\n"
"********************************************************")
# Trigger set-up
test_object.trigger.value = 0.0

# Fault set-up
test_object.fault_function.name = "Error fault"
test_object.fault_function.type = 20

trick.stop(0)
