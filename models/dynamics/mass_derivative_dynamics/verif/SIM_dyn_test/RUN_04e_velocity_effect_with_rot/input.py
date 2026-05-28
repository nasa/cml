exec(open("RUN_03_trans_motion/input.py").read())
add_3array( drg1, "vehicle.test_dyn.pseudo_dv")
vehicle.test_dyn.set_include_velocity_effect(True)
