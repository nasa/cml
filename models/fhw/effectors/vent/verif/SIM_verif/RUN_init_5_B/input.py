exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vent_params.py").read())
log_full_params()
drg_params.add_variable("ventset_obj.extra_vent.duration_set")

# section 4.2.5 -- initialization
# 5. Consolidate force-magnitude, impulse-magnitude, and duration
#    B. Otherwise, (duration has not been set), the duration is not assigned; 
#       this is to support the mode in which a vent is left open until closed
#       with a stop_vent() command.
# Note that the force-magnitude is set using the mass-flowrate and exhaust speed.

ventset_obj.extra_vent.unset_duration()
ventset_obj.extra_vent.use_dynamic_mode()
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\n"
"***************************************************************************\n"
"Pre-init settings:\n"
"force_magnitude   : %lf N\n"
"impulse_magnitude : %lf Ns\n"
"duration          : %lf s\n"
"duration-set      : %d\n"
"force-mag-set     : %d\n"
"***************************************************************************\n"
%(ventset_obj.force_magnitude,
  ventset_obj.impulse_magnitude,
  ventset_obj.duration,
  ventset_obj.duration_set,
  ventset_obj.force_magnitude_set))


trick.add_read(0.0, '''
ventset_obj.copy_parameters(ventset_obj.extra_vent)

print("\\n"
"***************************************************************************\\n"
"Post-init settings:\\n"
"force_magnitude   : %lf N\\n"
"impulse_magnitude : %lf Ns\\n"
"duration          : %lf s\\n"
"duration-set      : %d\\n"
"force-mag-set     : %d\\n"
"***************************************************************************\\n"
%(ventset_obj.force_magnitude,
  ventset_obj.impulse_magnitude,
  ventset_obj.duration,
  ventset_obj.duration_set,
  ventset_obj.force_magnitude_set))
''')
trick.stop(0)
