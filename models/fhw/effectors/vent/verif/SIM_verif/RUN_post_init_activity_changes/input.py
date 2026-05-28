exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vents.py").read())
log_add_state()

# Reduce the size of the vent influence to reduce the complications of rapidly-
# varying attitude modifying the inertial direction of the force and torque.
# Give vent0 a fixed duration
# Reduce mass depletion to avoid fully depleting the tanks.
ventset_obj.set_2_tanks.get_vent(0).set_force_vector([0.01, 0, 0])
ventset_obj.set_2_tanks.get_vent(0).set_duration(3)
ventset_obj.set_2_tanks.get_vent(0).set_flowrate(0.1)

ventset_obj.set_2_tanks.get_vent(1).set_impulse_vector([0, 0.01, 0])
ventset_obj.set_2_tanks.get_vent(1).set_exhaust_speed(0.1)

ventset_obj.extra_vent.set_flowrate(0.05) # s
ventset_obj.extra_vent.set_exhaust_speed(0.2)

ventset_obj.extra_simple_vent.set_duration(3)
ventset_obj.extra_simple_vent.set_impulse_magnitude(0.03)

trick.add_read(1, "ventset_obj.set_2_tanks.start_vents()")
trick.add_read(3, 'ventset_obj.set_2_tanks.unsubscribe()')
trick.add_read(5, """
print("\\n"
"**************************************************************************\\n"
"Error (x2) -- trying to start vents while vent-set is inactive\\n"
"**************************************************************************")
ventset_obj.set_2_tanks.start_vents()
ventset_obj.set_2_tanks.start_vent(0)
print(
"**************************************************************************\\n")
""")
trick.add_read(7, 'ventset_obj.set_2_tanks.subscribe()')
trick.add_read(8, 'ventset_obj.set_2_tanks.start_vents()')
trick.add_read(9, '''
ventset_obj.set_2_tanks.get_vent(0).active = False
ventset_obj.set_2_tanks.get_vent(1).active = False
ventset_obj.extra_vent.active = False
ventset_obj.extra_simple_vent.active = False
''')
trick.add_read(10, '''
ventset_obj.set_2_tanks.get_vent(0).active = True
ventset_obj.set_2_tanks.get_vent(1).active = True
ventset_obj.extra_vent.active = True
ventset_obj.extra_simple_vent.active = True
''')
trick.add_read(11, '''
print("\\n"
"**************************************************************************\\n"
"Error -- trying to start vent while inactive\\n"
"**************************************************************************")
ventset_obj.set_2_tanks.get_vent(0).active = False
ventset_obj.set_2_tanks.start_vent(0)
print(
"**************************************************************************\\n")
''')
trick.stop(12)
