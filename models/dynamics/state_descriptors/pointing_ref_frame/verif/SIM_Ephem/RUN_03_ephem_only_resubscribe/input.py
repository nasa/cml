# RUNs for this sim track the earth and sun positions.
# A vehicle is initialized on the earth-sun vector, and maintains the same
# earth-inertial state throughout.
# The pointing-frame is earth-to-sun.

# RUN_03_ephem_only_resubscribe runs similarly to RUN_01, with only the
# Ephem-based pointing frame and accompanying relative-state.
# Like FAIL_noephem_with_subscriptions, the frame is deactivated at sim-start and re-activated some time
# later.  Unlike FAIL_noephem_with_subscriptions, the re-activation is successful.

# NOTE - FOR THIS SIM, the model must be subscribed at initialization.  This is
#        only because the pointing frame is required when this S_define
#        initializes the relative state.
#        Initial subscription to the frame is not a requirement for the
#        pointing-frame model per se; without the relative-state, or with a
#        delayed initialization of the relative state, it would not be
#        necessary to subscribe the model until it is actually needed.
exec(open("common_input.py").read())
test.ephem_rotating_frame.subscribe()
trick.add_read(0, 'test.ephem_rotating_frame.unsubscribe()')
trick.add_read(300000, 'test.ephem_rotating_frame.subscribe()')
