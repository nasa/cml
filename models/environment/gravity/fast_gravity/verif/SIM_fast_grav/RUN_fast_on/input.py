exec(open("Log_data/log_data.py").read())
log_test_data(100)
log_fast_grav(100)

exec(open("Modified_data/env_setup.py").read())

vehicle.grav_controls_earth.degree = 16
vehicle.grav_controls_earth.order = 16

exec(open("Modified_data/fast_grav_setup.py").read())

vehicle.grav_controls_earth.reset_count_limit();

trick.sim_services.exec_set_terminate_time(16200)
