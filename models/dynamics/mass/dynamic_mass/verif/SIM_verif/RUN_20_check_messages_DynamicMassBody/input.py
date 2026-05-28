exec(open("RUN_01_nominal/input.py").read())

#Set suppression level high enough to see Inform() messages
trick.CMLMessage.set_publish_level (trick.CMLMessage.Inform)

#pick up error at entry to update_mass, dynamic_mass_body.cc:172
vehicle.tanks[0].update_mass()

# pick up warn at dynamic_mass_body:277
vehicle.test_set_initial_position(None)

# pick up error at dynamic_mass_body:302
vehicle.test_set_CM_offset(None)

# pick up inform at dynamic_mass_body:309
vehicle.tanks[0].set_CM_offset( [1,1,1])

# pick up inform at dynamic_mass_body_interpolation.cc:64
vehicle.tanks[0].dynamic_properties.interpolation.initialize()


# pick up an initialization warning at dynamic_mass_body:87
vehicle.tanks[0].interp_enabled = True;


# pick up messages at (in order)
# dyn_mass_body.cc:67
# dyn_mass_body.cc:223
trick.add_read(0.0, """
vehicle.tanks[0].initialize_dyn_mass()

vehicle.tanks[0].interp_enabled = True;
vehicle.tanks[0].update_mass()

""")


# run case 3 of initialize_dyn_mass() (dynamic_mass_body.cc:171-172)
# set consumable_mass to be greater than residual_mass to compute a
# negative residual_mass to force an error.
print("overriding vehicle.tanks[1].dynamic_properties.consumable_mass...")
vehicle.tanks[1].dynamic_properties.consumable_mass = 15.0
print("** new vehicle.tanks[1].dynamic_properties.consumable_mass=%lf"\
%vehicle.tanks[1].dynamic_properties.consumable_mass)
# trigger initialization of dynamic mass body early to
# pick up error at dynamic_mass_body.cc:184
# also picks up these other errors first because this body has not
# yet been properly initialized by the normal means:
#  dynamic_mass_body:79
#  dynamic_mass_body:105
print("triggering initialization of dynamic mass body early...")
vehicle.tanks[1].initialize_dyn_mass()
