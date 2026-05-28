exec(open("RUN_internal_no_update/input.py").read())

trick.add_read(5,"vehicle.test_internal_deriv.unsubscribe()")
trick.add_read(10,"vehicle.test_internal_deriv.subscribe()")
trick.add_read(15,"vehicle.test_internal_deriv.unsubscribe()")

trick.add_read(20,"""
print('*******************************************************************')
print('Disabling test_internal_deriv will produce instruction to remove it')
print('from the integration group, but it was never added so will get an')
print('error message from dyn_body.cc.')
print('*******************************************************************')
vehicle.test_internal_deriv.disable()
""")

trick.add_read(25,"""
print('****************************************************************************')
print('Disabling test_internal_deriv will prevent subsequent subscription to it.')
print('Attempting to subscribe will result in an error message.')
print('****************************************************************************')
vehicle.test_internal_deriv.subscribe()
""")

# pure code-coverage, trying to initialize a disabled model should return
# without either action or error.
trick.add_read(29,"vehicle.test_internal_deriv.initialize()")








