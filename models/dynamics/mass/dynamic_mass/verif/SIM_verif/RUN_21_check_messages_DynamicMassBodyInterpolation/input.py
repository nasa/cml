exec(open("RUN_01_nominal/input.py").read())

# pick up inform at dynamic_mass_body_interpolation.cc:64
vehicle.tanks[0].dynamic_properties.interpolation.initialize()

# pick up the no-table-available initialization warnings resulting from lines:
#  88, 100, 112, 124, 136, 148, 160, 172, 184
vehicle.add_mass_data()
