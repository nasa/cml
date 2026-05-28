exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vent_params.py").read())
log_full_params()

# section 4.2.5 -- initialization
# 4. If impulse vector has been specified but impulse magnitude has not, the magnitude is obtained from the vector.

ventset_obj.extra_vent.unset_direction()
ventset_obj.extra_vent.set_impulse_vector([1.0, 4.0, 8.0])

ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\n"
"***************************************************************************\n"
"Pre-init settings:\n"
"impulse             : [%lf, %lf, %lf] Ns\n"
"impulse_magnitude   : %lf Ns\n"
"impulse-set         : %d\n"
"impulse-mag-set     : %d\n"
"***************************************************************************\n"
%(ventset_obj.impulse[0], ventset_obj.impulse[1], ventset_obj.impulse[2],
  ventset_obj.impulse_magnitude,
  ventset_obj.impulse_set,
  ventset_obj.impulse_magnitude_set))


trick.add_read(0.0, '''
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\\n"
"***************************************************************************\\n"
"Post-init settings:\\n"
"impulse             : [%lf, %lf, %lf] Ns\\n"
"impulse_magnitude   : %lf Ns\\n"
"impulse-set         : %d\\n"
"impulse-mag-set     : %d\\n"
"***************************************************************************\\n"
%(ventset_obj.impulse[0], ventset_obj.impulse[1], ventset_obj.impulse[2],
  ventset_obj.impulse_magnitude,
  ventset_obj.impulse_set,
  ventset_obj.impulse_magnitude_set))
''')
trick.stop(0)
