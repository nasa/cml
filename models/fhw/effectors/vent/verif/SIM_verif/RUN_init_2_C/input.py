exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vent_params.py").read())
log_full_params()

# section 4.2.5 -- initialization
# 2. The direction is generated
#   C. Otherwise, if the impulse-vector has been specified, it is used for direction


ventset_obj.extra_vent.unset_direction()
ventset_obj.extra_vent.set_impulse_vector([0.0, -8.0, 0.0])


ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\n"
"***************************************************************************\n"
"Pre-init settings:\n"
"direction         : [%lf, %lf, %lf] \n"
"force             : [%lf, %lf, %lf] N\n"
"impulse           : [%lf, %lf, %lf] N\n"
"direction-set     : %d\n"
"force-set         : %d\n"
"***************************************************************************\n"
%(ventset_obj.direction[0], ventset_obj.direction[1], ventset_obj.direction[2],
  ventset_obj.force[0], ventset_obj.force[1], ventset_obj.force[2],
  ventset_obj.impulse[0], ventset_obj.impulse[1], ventset_obj.impulse[2],
  ventset_obj.direction_set,
  ventset_obj.force_set))

trick.add_read(0.0, '''
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\\n"
"***************************************************************************\\n"
"Post-init settings:\\n"
"direction         : [%lf, %lf, %lf] \\n"
"force             : [%lf, %lf, %lf] N\\n"
"impulse           : [%lf, %lf, %lf] N\\n"
"direction-set     : %d\\n"
"force-set         : %d\\n"
"***************************************************************************\\n"
%(ventset_obj.direction[0], ventset_obj.direction[1], ventset_obj.direction[2],
  ventset_obj.force[0], ventset_obj.force[1], ventset_obj.force[2],
  ventset_obj.impulse[0], ventset_obj.impulse[1], ventset_obj.impulse[2],
  ventset_obj.direction_set,
  ventset_obj.force_set))
''')
trick.stop(0)
