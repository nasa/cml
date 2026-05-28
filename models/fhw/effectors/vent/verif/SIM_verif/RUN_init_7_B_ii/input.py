# section 4.2.5 -- initialization
# 7. The impulse vector is generated
#   B. Otherwise (i.e. impulse-magnitude not known), then:
#     ii. If in dynamic mode with indefinite duration, the impulse may remain
#         unassigned at this point; the impulse value is not defined while
#         operating in such a configuration so this is a desirable outcome.

exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vent_params.py").read())
log_full_params()

# remove information about impulse by:
# unset duration to remove F * dt option
ventset_obj.extra_vent.unset_duration()
# Run in dynamic mode
ventset_obj.extra_vent.use_dynamic_mode()

ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\n"
"***************************************************************************\n"
"Pre-init settings:\n"
"direction         : [%lf, %lf, %lf] \n"
"impulse           : [%lf, %lf, %lf] Ns\n"
"impulse-magnitude : %lf Ns\n"
"direction-set     : %d\n"
"impulse-set       : %d\n"
"***************************************************************************\n"
%(ventset_obj.direction[0], ventset_obj.direction[1], ventset_obj.direction[2],
  ventset_obj.impulse[0], ventset_obj.impulse[1], ventset_obj.impulse[2],
  ventset_obj.impulse_magnitude,
  ventset_obj.direction_set,
  ventset_obj.impulse_set))



trick.add_read(0.0, '''
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\\n"
"***************************************************************************\\n"
"Post-init settings:\\n"
"direction         : [%lf, %lf, %lf] \\n"
"impulse           : [%lf, %lf, %lf] Ns\\n"
"impulse-magnitude : %lf Ns\\n"
"direction-set     : %d\\n"
"impulse-set       : %d\\n"
"***************************************************************************\\n"
%(ventset_obj.direction[0], ventset_obj.direction[1], ventset_obj.direction[2],
  ventset_obj.impulse[0], ventset_obj.impulse[1], ventset_obj.impulse[2],
  ventset_obj.impulse_magnitude,
  ventset_obj.direction_set,
  ventset_obj.impulse_set))
''')
trick.stop(0)
