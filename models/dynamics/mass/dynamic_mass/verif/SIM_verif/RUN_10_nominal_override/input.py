exec(open("RUN_01_nominal/input.py").read())
vehicle.tanks[0].nominal_properties.set_data(vehicle.nominal_override)

trick.stop(3)
