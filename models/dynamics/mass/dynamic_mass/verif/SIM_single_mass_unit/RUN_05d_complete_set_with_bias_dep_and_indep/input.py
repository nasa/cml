exec(open("RUN_05a_complete_set_with_bias/input.py").read())
mass_test.tank.dynamic_properties.interpolation.tab_cg_flag = True
mass_test.tank.dynamic_properties.interpolation.tab_moi_flag = True
mass_test.tank.dynamic_properties.interpolation.tab_poi_flag = True

mass_test.tank.dynamic_properties.cg_bias =  [1.0, 2.0, 3.0]
mass_test.tank.dynamic_properties.moi_bias = [-1.0, -2.0, -3.0]
mass_test.tank.dynamic_properties.poi_bias = [11.0, 12.0, 13.0]
