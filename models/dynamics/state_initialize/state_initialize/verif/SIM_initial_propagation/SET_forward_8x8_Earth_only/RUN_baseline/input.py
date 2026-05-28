exec(open("common_input.py").read())
log_test_data(10.0)
test.prop_time = 0.0

trick.sim_services.exec_set_terminate_time(6010)

test.grav_controls_earth.degree = 8
test.grav_controls_earth.order = 8
test.grav_controls_sun.active = False
test.grav_controls_moon.active = False
