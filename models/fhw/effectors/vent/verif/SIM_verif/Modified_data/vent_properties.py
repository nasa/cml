# Set the properties for the two created vents in the set_2_tanks vent-set
# and the two independent vents by default.
# Any parameters that should not be set can be "unset" in the appropriate
# testing area.
ventset_obj.set_2_tanks.get_vent(0).name = "set_2_tanks.vent[0]"
ventset_obj.set_2_tanks.get_vent(0).set_force_vector([-8.0, 0.0, 0.0]) # N
ventset_obj.set_2_tanks.get_vent(0).set_flowrate(0.25) # kg/s

ventset_obj.set_2_tanks.get_vent(1).name = "set_2_tanks.vent[1]"
ventset_obj.set_2_tanks.get_vent(1).set_impulse_vector([0.0, -8.0, 0.0]) # N*s
ventset_obj.set_2_tanks.get_vent(1).set_exhaust_speed(32.0) # m/s
ventset_obj.set_2_tanks.get_vent(1).use_impulse_mode()


ventset_obj.extra_vent.name = "extra_vent"
ventset_obj.extra_vent.set_direction([0.0, 0.0, 1.0])
ventset_obj.extra_vent.set_flowrate(0.25) # kg/s
ventset_obj.extra_vent.set_exhaust_speed(16.0) # m/s
ventset_obj.extra_vent.set_duration(2.0) # s
ventset_obj.extra_vent.use_impulse_mode()

ventset_obj.extra_simple_vent.name = "extra_simple_vent"
ventset_obj.extra_simple_vent.set_impulse_magnitude(40.0) # N s
ventset_obj.extra_simple_vent.set_duration(5.0) # s
ventset_obj.extra_simple_vent.set_direction([0.0, 0.0, -1.5]) # Non-unit vector to test for normalization
