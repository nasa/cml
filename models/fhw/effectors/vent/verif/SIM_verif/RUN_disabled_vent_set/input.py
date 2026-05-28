exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vents.py").read())
ventset_obj.set_2_tanks.disable()
print("\n"
"**************************************************************************\n"
"Error -- trying to start vents while vent-set is inactive\n"
"**************************************************************************")
ventset_obj.set_2_tanks.start_vents()
trick.stop(2)
