# RUNs for this sim track the earth and sun positions.
# A vehicle is initialized on the earth-sun vector, and maintains the same
# earth-inertial state throughout.
# The pointing-frame is earth-to-sun.

# RUN_01_ephem_only runs only the Ephem-based pointing frame and accompanying
# relative-state.
# This is the recommended implementation for a pointing reference frame that
# relies on the presence of frames managed by the Ephemerides Manager.
exec(open("common_input.py").read())
test.ephem_rotating_frame.subscribe()
