exec(open("RUN_01_baseline/input.py").read())
log_basic_addon()
add_3array( drg1, "vehicle.test_dyn.pseudo_dv")
vehicle.rot_init.ang_velocity[2] = 0.0
trick.stop(25.0)
