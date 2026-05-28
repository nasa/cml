exec(open("RUN_01_nominal/input.py").read())
trick.add_read(0, "vehicle.tanks[0].set_initial_position(False, [-2,0,0])")

trick.stop(3)
