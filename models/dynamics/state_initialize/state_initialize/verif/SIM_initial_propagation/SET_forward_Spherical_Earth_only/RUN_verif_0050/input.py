exec(open("Log_data/log_data.py").read())
exec(open("common_input.py").read())
common_input(50)
test.grav_controls_earth.spherical = True
test.grav_controls_sun.active = False
test.grav_controls_moon.active = False
