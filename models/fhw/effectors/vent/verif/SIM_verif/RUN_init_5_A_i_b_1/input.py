exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vent_params.py").read())
log_full_params()

# section 4.2.5 -- initialization
# 5. Consolidate force-magnitude, impulse-magnitude, and duration
#   A. If duration has been set:
#     i. If force-magnitude has been set
#       b. if impulse-magnitude has not been set OR operating in dynamic mode,
#          compute the impulse-magnitude from force-magnitude and direction.

ventset_obj.extra_vent.use_dynamic_mode()
ventset_obj.extra_vent.set_force_magnitude(3)
ventset_obj.extra_vent.unset_flowrate()
ventset_obj.extra_vent.set_impulse_magnitude(9)
ventset_obj.extra_vent.set_duration(2)

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


print("\n"
"*********************************************************\n"
"Warning message related to overconstraint and\n"
"overriding impulse-magnitude.\n"
"*********************************************************")

trick.add_read(0.0, '''
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print(
"*********************************************************\\n"
"Return to test\\n"
"*********************************************************")

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
