exec(open("RUN_02_interp/input.py").read())

#Set suppression level high enough to Inform() messages
trick.CMLMessage.set_publish_level (trick.CMLMessage.Inform)

# pick up initialization warn regarding non-interoplation when data is present
#  warnings specified at dynamic_mass_body_interpolation.cc:417, called from
#  lines 82, 94, 106, 118, 130, 142, 154
mass_test.add_poi_data()
mass_test.tank.dynamic_properties.interpolation.set_interp_position_master(False)
mass_test.tank.dynamic_properties.interpolation.set_interp_inertia_master(False)

# pick up inform at dynamic_mass_body_interpolation.cc:71
trick.add_read(0,"""
mass_test.tank.dynamic_properties.interpolation.initialize()
""")
