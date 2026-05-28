exec(open("common_input.py").read())

vehicle.test_internal_deriv.subscribe()

vehicle.test_internal_deriv.deriv[0] = 1.0
vehicle.test_internal_deriv.deriv[1] = 2.0
vehicle.test_internal_deriv.deriv[2] = 3.0

log_add_3vec( dr_group, "vehicle.test_internal_deriv.variable")
