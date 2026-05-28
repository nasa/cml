exec(open("RUN_01_nominal/input.py").read())

#Set suppression level high enough to see Inform() messages
trick.CMLMessage.set_publish_level (trick.CMLMessage.Inform)

# pick up error at dynamic_mass_group.cc:117
vehicle.group_alt.initialize()

# pick up inform at dynamic_mass_group.cc:107
vehicle.group_alt.disable()
vehicle.group_alt.initialize()

# pick up warn at dynamic_mass_group.cc:268
# Test twice to ensure we only get 1 message
vehicle.group_alt.series_flow(1,2)
vehicle.group_alt.series_flow(1,2)

# This was copied from RUN_08_dry_mass. Testing the scenario
# when "revert_dry_mass_config()" is called without previously
# calling "initiate_dry_mass_config()". Expect an error!
trick.add_read(2.0,"""
vehicle.tanks[0].dynamic_properties.mass_consumed_step = 1.23

# do not call "initiate_dry_mass_config()" to
# pick up error at dynamic_mass_group.cc::392
#vehicle.group.initiate_dry_mass_config()

vehicle.group.revert_dry_mass_config()
""")
