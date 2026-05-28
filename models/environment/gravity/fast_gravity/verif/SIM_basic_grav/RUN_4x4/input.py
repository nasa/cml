exec(open("Log_data/log_data.py").read())
log_test_data(100)

exec(open("Modified_data/env_setup.py").read())

vehicle.grav_controls_earth.degree = 4 
vehicle.grav_controls_earth.order = 4

trick.sim_services.exec_set_terminate_time(16200)
