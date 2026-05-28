# This run tests the effect of the dry-mass configuration operation.
# With this operation, the composite-body mass can be momentarily set to
# represent the dry-mass of the vehicle (i.e. consumable-mass = 0 for all
# bodies)
# After reverting the configuration, the mass properties should be identical to
# their previous values.
exec(open("RUN_01_nominal/input.py").read())
trick.add_read(2.0,"""
vehicle.tanks[0].dynamic_properties.mass_consumed_step = 1.23
print("Wet mass at t = 2.0")
print("mass: ", vehicle.body.mass.composite_properties.mass)
print("position: ", vehicle.body.mass.composite_properties.position)
print("inertia: ", vehicle.body.mass.composite_properties.inertia)
print("mass_consumed_step", vehicle.tanks[0].dynamic_properties.mass_consumed_step)

vehicle.group.initiate_dry_mass_config()

print("")
print("Dry mass at t = 2.0")
print("mass: ", vehicle.body.mass.composite_properties.mass)
print("position: ", vehicle.body.mass.composite_properties.position)
print("inertia: ", vehicle.body.mass.composite_properties.inertia)
print("mass_consumed_step", vehicle.tanks[0].dynamic_properties.mass_consumed_step)

vehicle.group.revert_dry_mass_config()

print("")
print("Wet mass at t = 2.0")
print("mass: ", vehicle.body.mass.composite_properties.mass)
print("position: ", vehicle.body.mass.composite_properties.position)
print("inertia: ", vehicle.body.mass.composite_properties.inertia)
print("mass_consumed_step", vehicle.tanks[0].dynamic_properties.mass_consumed_step)
vehicle.tanks[0].dynamic_properties.mass_consumed_step = 0.0
""")
