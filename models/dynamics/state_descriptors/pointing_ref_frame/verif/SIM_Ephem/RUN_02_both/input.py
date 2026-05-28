# RUNs for this sim track the earth and sun positions.
# A vehicle is initialized on the earth-sun vector, and maintains the same
# earth-inertial state throughout.
# The pointing-frame is earth-to-sun.

# RUN_02_both runs the Ephem-based pointing frame and accompanying
# relative-state AND the simpler pointing-frame with no Ephemerides connections.
# Because the ephemerides tree is established by the activation of the
# ephem-rotating-frame, the non-ephem-rotating-frame instance will work because
# it uses the same frames.
# But see the effect of this hidden dependency in FAIL_noephem_only
# when the ephem-rotating-frame is removed.
exec(open("common_input.py").read())
test.use_non_ephem = True
test.ephem_rotating_frame.subscribe()
test.non_ephem_rotating_frame.subscribe()
