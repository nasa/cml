exec(open("RUN_01_straight/input.py").read())

#Set suppression level high enough to see Inform() messages
trick.CMLMessage.set_publish_level (trick.CMLMessage.Inform)

# pick up initialization inform at dynamic_mass_body.cc:77
# mass_test.tank.interp_enabled = False
# Note - this should be automatic, and it is.

# Deliberately fault the interpolation model
# pick up the error at dynamic_mass_body.cc:188
trick.add_read(5,"""
mass_test.tank.interp_enabled = True
mass_test.tank.dynamic_properties.interpolation.table_lookup_set.unsubscribe()
""")
