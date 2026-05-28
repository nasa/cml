exec(open("Modified_data/env_setup.py").read())
ventset_obj.set_2_tanks.start_vents_at_activation = True

ventset_obj.set_2_tanks.get_vent(0).set_duration(2)
print("\n"
"**********************************************************************\n"
"Error (x2): start/stop vent with invalid index.\n"
"**********************************************************************")
trick.add_read(0,"""
ventset_obj.set_2_tanks.start_vent(9)
ventset_obj.set_2_tanks.stop_vent(9)
""")
