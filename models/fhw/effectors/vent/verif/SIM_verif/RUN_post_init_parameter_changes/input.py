# This run tests the capability to adjust vent parameters mid-run.

exec(open("Modified_data/env_setup.py").read())
exec(open("Modified_data/vent_properties.py").read())
exec(open("Log_data/log_vent_params.py").read())
log_full_params("ventset_obj.extra_vent")
log_flags("ventset_obj.extra_vent")

#Testing the content of Table 3.1 in documentation

# Start in impulse mode.

#Test 1 -- change direction
trick.add_read(1.0,"""
ventset_obj.extra_vent.set_direction([0,1,0])
""")

#Test 2 -- change duration
trick.add_read(2.0,"""
ventset_obj.extra_vent.set_duration(4)
""")

#Test 3 -- change force-magnitude, delta-t known, impulse mode
trick.add_read(3,"""
ventset_obj.extra_vent.set_force_magnitude(6)
""")

#Test 4 -- change force-magnitude, delta-t not known, impulse mode
trick.add_read(4,"""
ventset_obj.extra_vent.unset_duration()
print("\\n"
"************************************************************************\\n"
"Error Message at t=4: cannot adjust impulse without knowing delta-t.\\n"
"************************************************************************")
ventset_obj.extra_vent.set_force_magnitude(8)
""")

#Test 5 -- change  force-magnitude, hold I, I known
trick.add_read(5,"""
ventset_obj.extra_vent.set_force_magnitude(10, True)
""")

#Test 6 -- change force-magnitude, hold I, I not known
trick.add_read(6,"""
ventset_obj.extra_vent.unset_impulse_magnitude()
print("\\n"
"************************************************************************\\n"
"Error Message at t=6: cannot set duration without knowing impulse.\\n"
"************************************************************************")
ventset_obj.extra_vent.set_force_magnitude(4, True)
""")

#Test 7 -- change force-magnitude, hold I, I not known, duration unset
trick.add_read(7,"""
print("\\n"
"************************************************************************\\n"
"Error Message at t=7: cannot set duration without knowing impulse.\\n"
"************************************************************************")
ventset_obj.extra_vent.set_force_magnitude(4, True)
""")

#Test 8 -- change force-magnitude to 0.0, hold I, I not known, duration unset
trick.add_read(8,"""
print("\\n"
"************************************************************************\\n"
"Error Message at t=8: cannot set duration without knowing impulse.\\n"
"************************************************************************")
ventset_obj.extra_vent.set_force_magnitude(0, True)
""")

#Test 9 -- change:
#            flowrate while holding to unspecified exhaust-speed
#            impulse magnitude with force unset (retaining the zero-value for
#                force here would result in the exhaust speed going to
#                zero also)
#            force-magnitude with unknown exhaust-speed and known flowrate
trick.add_read(9,"""
ventset_obj.extra_vent.unset_exhaust_speed()
ventset_obj.extra_vent.set_flowrate(0.2)
ventset_obj.extra_vent.unset_force_magnitude()
ventset_obj.extra_vent.set_impulse_magnitude(16, False)
ventset_obj.extra_vent.set_force_magnitude(4, True)
""")

#Test 10 -- change force vector
trick.add_read(10,"""
ventset_obj.extra_vent.set_force_vector([0,0,6])
""")

#Test 11 -- change impulse-magnitude, hold to the specified force-magnitude
trick.add_read(11,"""
ventset_obj.extra_vent.set_impulse_magnitude(18)
""")

#Test 12 -- change impulse-magnitude, hold to an unspecified force-magnitude
trick.add_read(12,"""
ventset_obj.extra_vent.unset_force_magnitude()
ventset_obj.extra_vent.set_impulse_magnitude(15)
""")

#Test 13 -- change impulse-magnitude, do not hold Force-magnitude, duration known
trick.add_read(13,"""
ventset_obj.extra_vent.set_impulse_magnitude(12, False)
""")

#Test 14 -- change impulse-magnitude, do not hold Force-magnitude, duration known
#           exhaust speed not known.
trick.add_read(14,"""
ventset_obj.extra_vent.unset_exhaust_speed()
ventset_obj.extra_vent.set_impulse_magnitude(24, False)
""")

#Test 15 -- change impulse vector (hold F-mag)
trick.add_read(15,"""
ventset_obj.extra_vent.set_impulse_vector([12,16,0])
""")

#Test 16 -- change flowrate
trick.add_read(16,"""
ventset_obj.extra_vent.set_flowrate(0.1)
""")

#Test 17 -- change flowrate, with exhaust-speed floating
trick.add_read(17,"""
ventset_obj.extra_vent.set_flowrate(0.2, False)
""")

#Test 18 -- change exhaust-speed (holding flowrate)
trick.add_read(18,"""
ventset_obj.extra_vent.set_exhaust_speed(10)
""")

#Test 19 -- change exhaust-speed with flowrate floating
trick.add_read(19,"""
ventset_obj.extra_vent.unset_flowrate()
ventset_obj.extra_vent.set_exhaust_speed(20)
""")

#Test 20 -- change exhaust-speed with flowrate floating
trick.add_read(20,"""
ventset_obj.extra_vent.set_exhaust_speed(40, False)
""")

#Test 21 -- change to dynamic mode and start vents
trick.add_read(21,"""
print("\\n"
"************************************************************************\\n"
"Warning Message at t=21:\\n"
"   Initially, duration was assigned to last for 2.0s.\\n"
"   During changes above, it has been automatically reassigned to a new\\n"
"   value for self-consistency.\\n"
"************************************************************************")
ventset_obj.set_2_tanks.get_vent(0).set_duration(5)
ventset_obj.extra_vent.use_dynamic_mode()
ventset_obj.set_2_tanks.start_vents()
""")

#Test 22 -- change duration
trick.add_read(22,"""
ventset_obj.extra_vent.set_duration(5)
""")

#Test 23 -- change duration
trick.add_read(23,"""
ventset_obj.extra_vent.set_duration(10, False)
""")

#Test 24 -- change force-magnitude to 0.0, hold I, duration becomes unset
trick.add_read(24,"""
print("\\n"
"************************************************************************\\n"
"Error Message at t=24: cannot set duration with zero force and\\n"
"                       non-zero impulse.\\n"
"************************************************************************")
ventset_obj.extra_vent.set_force_magnitude(0, True)
""")

#Test 25 -- change force-magnitude with impulse-magnitude floating and duration
#           unset (dynamic mode)
trick.add_read(25,"""
ventset_obj.extra_vent.unset_duration()
ventset_obj.extra_vent.set_force_magnitude(4, False)
""")

#Test 26 -- change impulse-magnitude with force-magnitude floating and duration
#           unset (dynamic mode)
trick.add_read(26,"""
ventset_obj.extra_vent.unset_duration()
print("\\n"
"************************************************************************\\n"
"Warning Message at t=26: cannot set force from an impulse specification\\n"
"                         without knowing the duration.\\n"
"************************************************************************")
ventset_obj.extra_vent.set_impulse_magnitude(20, False)
""")

#Test 27 -- change impulse vector with F-mag not held, duration invalid.
trick.add_read(27,"""
print("\\n"
"************************************************************************\\n"
"Warning Message at t=27: cannot set force from an impulse specification\\n"
"                         without knowing the duration.\\n"
"************************************************************************")
ventset_obj.extra_vent.set_impulse_vector([10,0,0], False)
""")

#Test 28 -- change impulse vector with F-mag not held, duration valid.
trick.add_read(28,"""
ventset_obj.extra_vent.set_duration(5)
ventset_obj.extra_vent.set_impulse_vector([0,10,0], False)
""")

#Test 29 --  Code coverage to test the remaining unset* methods
trick.add_read(29,"""
ventset_obj.extra_vent.unset_direction()
ventset_obj.extra_vent.unset_force()
ventset_obj.extra_vent.unset_impulse()
ventset_obj.extra_vent.unset_exhaust_speed()
ventset_obj.extra_vent.unset_flowrate()
""")


trick.stop(29)
