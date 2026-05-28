exec(open("RUN_04_complete_set/input.py").read())
mass_test.tank.dynamic_properties.mass_bias = -1.0
mass_test.tank.dynamic_properties.interpolation.tab_mass_flag = True
mass_test.tank.dynamic_properties.mass_dispersion_flag = True

trick.add_read(0, """
mass_test.tank.dynamic_properties.disperse_mass_properties()
""")
