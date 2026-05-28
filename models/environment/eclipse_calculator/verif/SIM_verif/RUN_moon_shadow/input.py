exec(open("RUN_conical/input.py").read())

vehicle.body.integ_frame_name = "Moon.inertial"
vehicle.state_initialize.planet_name = "Moon"
vehicle.state_initialize.position = [-20.717E6, 89.749E6, 38.935E6]
trick.sim_services.exec_set_terminate_time(250000)
