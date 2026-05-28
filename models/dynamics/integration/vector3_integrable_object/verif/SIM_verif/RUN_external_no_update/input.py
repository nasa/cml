exec(open("common_input.py").read())

vehicle.test_external_deriv.subscribe()

vehicle.external_deriv[0] = 1.0
vehicle.external_deriv[1] = 2.0
vehicle.external_deriv[2] = 3.0

log_add_3vec( dr_group, "vehicle.test_external_deriv.variable")
