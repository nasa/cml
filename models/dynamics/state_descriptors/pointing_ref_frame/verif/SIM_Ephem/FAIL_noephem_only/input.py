# RUNs for this sim track the earth and sun positions.
# A vehicle is initialized on the earth-sun vector, and maintains the same
# earth-inertial state throughout.
# The pointing-frame is earth-to-sun.

# FAIL_noephem_only runs only the simpler pointing-frame with no
# Ephemerides connections.
# Because the simple implementation is not able to add the
# target/originating frame to the ephemeris tree, this will fail.
exec(open("common_input.py").read())
test.use_non_ephem = True
test.use_ephem = False
test.non_ephem_rotating_frame.subscribe()

print("*********************************************************")
print("Terminal error.  Ephem tree has not been built.")
print("Sun.inertial and Earth.inertial are not the same tree")
print("*********************************************************")
