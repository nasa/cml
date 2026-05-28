exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vent_params.py").read())
log_full_params()

# section 4.2.5 -- initialization
# 3. If force vector has been specified but force magnitude has not, the magnitude is obtained from the vector

ventset_obj.extra_vent.unset_flowrate()
ventset_obj.extra_vent.unset_direction()
ventset_obj.extra_vent.set_force_vector([1.0, 4.0, 8.0])

ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\n"
"***************************************************************************\n"
"Pre-init settings:\n"
"force             : [%lf, %lf, %lf] N\n"
"force_magnitude   : %lf N\n"
"force-set         : %d\n"
"force-mag-set     : %d\n"
"***************************************************************************\n"
%(ventset_obj.force[0], ventset_obj.force[1], ventset_obj.force[2],
  ventset_obj.force_magnitude,
  ventset_obj.force_set,
  ventset_obj.force_magnitude_set))


trick.add_read(0.0, '''
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\\n"
"***************************************************************************\\n"
"Post-init settings:\\n"
"force             : [%lf, %lf, %lf] N\\n"
"force_magnitude   : %lf N\\n"
"force-set         : %d\\n"
"force-mag-set     : %d\\n"
"***************************************************************************\\n"
%(ventset_obj.force[0], ventset_obj.force[1], ventset_obj.force[2],
  ventset_obj.force_magnitude,
  ventset_obj.force_set,
  ventset_obj.force_magnitude_set))
''')
trick.stop(0)
