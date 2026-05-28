exec(open("Modified_data/env_setup.py").read())
ventset_obj.set_2_tanks.get_vent(0).set_duration(2)

print("\n"
"**********************************************************************\n"
"Error: cannot start an uninitialized vent\n"
"**********************************************************************")
trick.add_read(0, """
ventset_obj.set_2_tanks.add_vent( ventset_obj.uninitialized_vent)
ventset_obj.set_2_tanks.start_vents()
""")
trick.stop(0)
