exec(open("RUN_02_interp/input.py").read())
mass_test.add_poi_data()
mass_test.tank.dynamic_properties.interpolation.set_interp_position(True, True,
True);
mass_test.tank.dynamic_properties.interpolation.set_interp_moi(True, True, True);
mass_test.tank.dynamic_properties.interpolation.set_interp_poi(True, True, True);
