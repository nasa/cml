exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vents.py").read())
log_add_activity("ventset_obj.extra_vent")


ventset_obj.extra_vent.use_dynamic_mode()
ventset_obj.extra_vent.unset_duration()
ventset_obj.extra_vent.set_direction([0.0, 1.0, 0.0])

print("\n"
"***************************************************************************\n"
"Warning message -- vent(0) has no duration and no indefinite-duration flag\n"
"***************************************************************************")

trick.add_read(1.0, "ventset_obj.set_2_tanks.start_vents()")

# Now just keep them open until mass depletes.
# vent(0) and extra_vent are both drawing 0.25 kg /s so the 1.0 kg available
# will last for 2 seconds.

trick.stop(4)
