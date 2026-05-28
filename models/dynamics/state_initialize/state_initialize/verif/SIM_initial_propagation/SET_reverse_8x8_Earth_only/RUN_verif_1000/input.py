exec(open("Log_data/log_data.py").read())
exec(open("common_input.py").read())
common_input_rev(1000)
test.grav_controls_earth.degree = 8
test.grav_controls_earth.order = 8
test.grav_controls_sun.active = False
test.grav_controls_moon.active = False
