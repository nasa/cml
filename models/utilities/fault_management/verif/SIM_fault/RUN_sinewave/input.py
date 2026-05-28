exec(open('RUN_linear/input.py').read())

# Switch the fault to a sinewave
test_object.fault_function.type = trick.FaultFunctionBase.Sinewave
test_object.trigger.value = 0.0

test_object.fault_function.set_param("nominal", 1)
test_object.fault_function.set_param("frequency", 0.25)
test_object.fault_function.set_param("amplitude", 1)
test_object.fault_function.set_param("phase_offset", 0.5)
test_object.fault_function.set_param("amplitude_rate", 0.2)
test_object.fault_function.set_param("phase_offset_rate", 0)

# t = 0, fault starts. Phase = 0.5

# change nominal phase-offset to 0. Should result in a half-wavelength
# phase-shift. Phase jumps from 1.0 to 0.5, Zero-crossing, +ve slope to -ve
trick.add_read(2.0, """
test_object.fault_function.set_param('phase_offset', 0)
""")

# set amp-rate to 0, this should drop amp to nominal 1. Phase = 1.75, Min.
trick.add_read(7, """
test_object.fault_function.set_param('amplitude_rate', 0)
""")

# set amp-rate to 0.1, based on dt = 11, this should set amp to 2.1 immediately.
#  Phase = 2.75, Min.
trick.add_read(11, """
test_object.fault_function.set_param('amplitude_rate', 0.1)
""")

# set amp-rate to 0 with the continuity flag. This should set the nominal mplitude
# to 2.3 and hold there. Phase = 3.25, Max.
trick.add_read(13, """
test_object.fault_function.set_param('amplitude_rate', 0, True)
""")

# increase frequency to 0.5, period is halved to 2. Phase=4.0, Zero crossing.
# Drop amplitude to 1.0 to help this section stand out.
trick.add_read(16, """
test_object.fault_function.set_param('amplitude', 1.0)
test_object.fault_function.set_param('frequency', 0.5)
""")

# change nominal amplitude to -1.0. Should result in the wave flipping upside
# down. Phase = 5.5, Zero crossing, slope changes from -ve to +ve
trick.add_read(19, """
test_object.fault_function.set_param('amplitude', -1.0)
""")

# change amplitude to +1.5 and frequency back to 0.25
# Phase = 6.5, zero-crossing but goes from a positive slope to a negative slope.
trick.add_read(21, """
test_object.fault_function.set_param('frequency', 0.25)
test_object.fault_function.set_param('amplitude', +1.5)
""")

# change phase-offset-rate to 1/16. With dt = 24, this should create an
# additional phase-offset of 1.5.  Phase goes from 7.25 to 8.75, Max to Min.
# With phase-offset-rate increasing pahse and phase advancing due to frequency
# integral this increases the apparent frequency.
# Change amplitude to 1.0 to help identify
trick.add_read(24, """
test_object.fault_function.set_param('phase_offset_rate', 1/16.0)
test_object.fault_function.set_param('amplitude', +1.0)
""")

# change phase-offset-rate back to 0. Phase drops from 10.0 to 8.25, zero to max.
trick.add_read(28, """
test_object.fault_function.set_param('phase_offset_rate', 0)
test_object.fault_function.set_param('amplitude', +1.5)
""")

# change phase-offset-rate to 0.25
# Phase goes from 9.25 to 17.25
# With phase-offset rate equal to frequency, this doubles the apparent frequency
# Set amplitude back to 1.0 to distinguish.
trick.add_read(32, """
test_object.fault_function.set_param('phase_offset_rate', 0.25)
test_object.fault_function.set_param('amplitude', +1.0)
""")

# Change phase offset rate back to 0 with the continuity flag, so nominal
# phase-offset is made to match current phase of 18.25.
# Adjust amplitude to 0.5.
trick.add_read(34, """
test_object.fault_function.set_param('phase_offset_rate', 0, True)
test_object.fault_function.set_param('amplitude', 0.5)
""")

# Change frequency-rate to -0.3 and nominal frequency to 10.4. Phase = 18.5
# frequency will now be negative (10.4-10.5 = -0.1) and phase will start to
# unwind. Slopoe will switch from negative to positive.
# Periods will grow shorter.
trick.add_read(35, """
test_object.fault_function.set_param('frequency', 10.4)
test_object.fault_function.set_param('frequency_rate', -0.3)
""")

# Stop the frequency from changing further, use the flag to hold it at its current
# level. Drop the amplitude again to identify the transition.
trick.add_read(42, """
test_object.fault_function.set_param('frequency_rate', 0, True)
test_object.fault_function.set_param('amplitude', 0.2)
""")



trick.stop(45)
