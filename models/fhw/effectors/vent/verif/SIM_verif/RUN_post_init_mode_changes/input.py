# This run tests the ability to switch between impulse and dynamic modes --
# at the vent level and the vent-set level.

exec(open("Modified_data/env_setup.py").read())
exec(open("Modified_data/vent_properties.py").read())
exec(open("Log_data/log_vent_params.py").read())
log_mode()

ventset_obj.extra_vent.unset_flowrate()
ventset_obj.extra_vent.unset_duration()
ventset_obj.extra_vent.set_impulse_magnitude(5)
ventset_obj.set_2_tanks.get_vent(0).set_duration(2)

trick.add_read(0, """
ventset_obj.copy_mode_flags()
""")

trick.add_read(1, """
print("\\n"
"*************************************************************************\\n"
"1. Error (x2) -- \'extra_vent\' not configured for dynamic mode.\\n"
"*************************************************************************")
ventset_obj.extra_vent.use_dynamic_mode()
ventset_obj.copy_mode_flags()
""")

trick.add_read(2, """
print("\\n\\n"
"*************************************************************************\\n"
"2. Success with Warning:\\n"
"    \'extra_vent\' transitions to dynamic mode.\\n"
"    New flowrate (with old impulse) overwrites duration specification.\\n"
"*************************************************************************")
ventset_obj.extra_vent.set_flowrate(0.25)
ventset_obj.extra_vent.use_dynamic_mode()
ventset_obj.copy_mode_flags()
""")

trick.add_read(3, """
print("\\n\\n"
"*************************************************************************\\n"
"3. Error -- \'extra_vent\' not configured for impulse mode (impulse vector).\\n"
"*************************************************************************")
ventset_obj.extra_vent.unset_impulse_magnitude()
ventset_obj.extra_vent.use_impulse_mode()
ventset_obj.copy_mode_flags()
""")

trick.add_read(4, """
print("\\n\\n"
"*************************************************************************\\n"
"4. Error -- \'extra_vent\' not configured for impulse mode (exhaust speed).\\n"
"*************************************************************************")
ventset_obj.extra_vent.set_impulse_magnitude(5)
ventset_obj.extra_vent.unset_exhaust_speed()
ventset_obj.extra_vent.use_impulse_mode()
ventset_obj.copy_mode_flags()
""")

trick.add_read(5, """
print("\\n\\n"
"*************************************************************************\\n"
"5. Success: -- \'extra_vent\' transitions to impulse mode.\\n"
"*************************************************************************")
ventset_obj.extra_vent.set_exhaust_speed(16.0)
ventset_obj.extra_vent.use_impulse_mode()
ventset_obj.copy_mode_flags()
""")

trick.add_read(6, """
print("\\n\\n"
"*************************************************************************\\n"
"6. Success -- transition all to impulse mode.\\n"
"*************************************************************************")
ventset_obj.set_2_tanks.use_impulse_mode()
ventset_obj.copy_mode_flags()
""")


trick.add_read(7,"""
print("\\n"
"*************************************************************************\\n"
"7. Error (x2) -- \'vent1\' not configured for dynamic mode.\\n"
"                 (No vents change mode)\\n"
"*************************************************************************")
ventset_obj.set_2_tanks.use_dynamic_mode()
ventset_obj.copy_mode_flags()
""")

trick.add_read(8,"""
print("\\n\\n"
"*************************************************************************\\n"
"8. Success with Warning:\\n"
"   -  All vents transition to dynamic mode.\\n"
"   - \'vent1\' new flowrate (with pre-existing impulse) sets new duration\\n"
"*************************************************************************")
ventset_obj.set_2_tanks.get_vent(1).set_flowrate(0.25)
ventset_obj.set_2_tanks.use_impulse_mode(False)
ventset_obj.copy_mode_flags()
""")


trick.add_read(9,"""
print("\\n\\n"
"*************************************************************************\\n"
"9. Success with Warning:\\n"
"   -  All vents transition to impulse mode.\\n"
"    \'extra_vent\' new duration sets new impulse specification.\\n"
"*************************************************************************")
ventset_obj.extra_vent.set_duration(2)
ventset_obj.set_2_tanks.use_impulse_mode()
ventset_obj.copy_mode_flags()
""")

trick.stop(9)
