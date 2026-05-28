exec(open("Modified_data/env_setup.py").read())
ventset_obj.set_2_tanks.get_vent(0).set_duration(2)

print("\n"
"**********************************************************************\n"
"Error(x2): cannot start a vent with unset parameters (impulse mode)\n"
"**********************************************************************")
trick.add_read(0, """
ventset_obj.extra_vent.unset_impulse_magnitude()
ventset_obj.set_2_tanks.start_vents()

ventset_obj.extra_vent.unset_exhaust_speed()
ventset_obj.set_2_tanks.start_vents()
""")

trick.add_read(1, """
print("\\n"
"**********************************************************************\\n"
"Error(x2): cannot start a vent with unset parameters (dynamic mode)\\n"
"**********************************************************************")
ventset_obj.extra_vent.set_exhaust_speed(10)
ventset_obj.extra_vent.use_dynamic_mode()
ventset_obj.extra_vent.unset_force_magnitude()
ventset_obj.set_2_tanks.start_vents()

ventset_obj.extra_vent.unset_flowrate()
ventset_obj.set_2_tanks.start_vents()

""")
trick.stop(1)

