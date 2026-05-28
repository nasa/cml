exec(open("RUN_01_nominal/input.py").read())

# pick up an initialization warning at dynamic_mass_group.cc:281
vehicle.group_alt.add_mass_to_group( vehicle.free_flying_body)
vehicle.group_alt.add_mass_to_group( vehicle.tanks[0])
vehicle.group_alt.initialize()
