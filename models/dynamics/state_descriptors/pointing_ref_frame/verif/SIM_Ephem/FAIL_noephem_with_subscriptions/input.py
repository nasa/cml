# RUNs for this sim track the earth, and sun positions.
# A vehicle is initialized on the earth-sun vector, and maintains the same
# earth-inertial state throughout.
# The pointing-frame is earth-to-sun.

# FAIL_noephem_with_subscriptions further illustrates the problem of
# using Ephem Frames without the EphemBasedPointingRefFrame.
# In this scenario, we emulate the lurking presence of another model in a
# simulation.
# The following pattern will, perhaps confusingly, lead to early termination:
# - Use the lurking model (we use the input file directly) to add the
#    sun inertial frame to the Ephemeris tree and subscribe to it; this allows
#    successful initialization of the PointingRefFrame.
# - Switch off the lurking model (again, we do this in the input file), thereby
#    deactivating the sun frame and removing it from the Ephemeris tree.
# - Reactivate the sun-inertial frame and switch on the pointing ref frame.
# Because the Ephem tree does not get rebuilt simply by reactivating the
# sun-inertial frame, this act is insufficient to put earth.inertial and
# sun.inertial in the same tree.  The sim will still terminate when it tries
# to compute the relative state between the originating and target frames.
# Conclusion -- When the model is subscribed, the Ephem tree must get rebuilt.
exec(open("common_input.py").read())
test.use_non_ephem = True
test.use_ephem = False
dynamics.dyn_manager.add_ref_frame( sun.planet.inertial)
sun.planet.inertial.subscribe()
test.non_ephem_rotating_frame.subscribe()

trick.add_read(0, '''
sun.planet.inertial.unsubscribe()
test.non_ephem_rotating_frame.unsubscribe()
''')

# RUN is expected to fail when test.non_ephem_rotating_frame.subscribe() is
# called, but because of a Trick internal issue, the runs can't fail properly
# in the Python layer through trick.add_read(...) calls. To bypass this, 
# a boolean variable, fail_noephem_with_subscriptions is used to trigger the 
# call to the function that causes the failure inside of the C++ layer.
trick.add_read(300000, '''
print("*********************************************************************")
print("Terminal error.  Sun.inertial is not in tree.")
print("Even though sun.inertial is newly active, the Ephemeris tree has not")
print("been rebuilt, so the frame does not exist in the tree.")
print("*********************************************************************")
sun.planet.inertial.subscribe()
test.fail_noephem_with_subscriptions = True
''')
