exec(open("RUN_06_set_mass_both/input.py").read())

vehicle.group.unsubscribe()
trick.add_read(4.0, "vehicle.group.subscribe()")
