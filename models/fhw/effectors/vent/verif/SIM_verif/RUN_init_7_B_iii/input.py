# section 4.2.5 -- initialization
# 7. The impulse vector is generated
#   B. Otherwise (i.e. impulse-magnitude not known), then:
#     iii. If in dynamic mode with specified duration, the impulse-magnitude 
#          would have been overridden in step 5-A-i-b and the function would 
#          have branched to step 7-A.

exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vent_params.py").read())
log_full_params()

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
