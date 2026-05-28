# section 4.2.5 -- initialization
# 5. Consolidate force-magnitude, impulse-magnitude, and duration
#    A. If duration has been set:
#       ii. Otherwise (force-mag not set), if the impulse-magnitude has been
#           set, compute the force-magnitude from impulse-magnitude and duration
exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vent_params.py").read())
log_full_params()

ventset_obj.extra_vent.use_dynamic_mode()
# unset flowrate to prevent computation of force-magnitude from m-dot v_e
ventset_obj.extra_vent.unset_flowrate()
ventset_obj.extra_vent.set_impulse_magnitude(9)

ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\n"
"***************************************************************************\n"
"Pre-init settings:\n"
"force_magnitude   : %lf N\n"
"impulse_magnitude : %lf Ns\n"
"duration          : %lf s\n"
"***************************************************************************\n"
%(ventset_obj.force_magnitude,
  ventset_obj.impulse_magnitude,
  ventset_obj.duration))

trick.add_read(0.0, '''
ventset_obj.copy_parameters(ventset_obj.extra_vent)

print("\\n"
"***************************************************************************\\n"
"Post-init settings:\\n"
"force_magnitude   : %lf N\\n"
"impulse_magnitude : %lf Ns\\n"
"duration          : %lf s\\n"
"***************************************************************************\\n"
%(ventset_obj.force_magnitude,
  ventset_obj.impulse_magnitude,
  ventset_obj.duration))
''')
trick.stop(0)
