exec(open("Modified_data/env_setup.py").read())


ventset_obj.extra_vent.set_force_vector([0,2,0])
ventset_obj.extra_vent.set_force_magnitude(1)
ventset_obj.extra_vent.set_impulse_vector([2,0,0])
ventset_obj.extra_vent.set_impulse_magnitude(1)
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\n"
"***************************************************************************\n"
"Error messages (x4) about overconstrained:\n"
" - force magnitude\n"
" - direction\n"
" - force vector\n"
" - impulse vector\n"
"***************************************************************************")


trick.add_read(0,"""
print("\\n"
"***************************************************************************\\n"
"Pre-init values:\\n"
"direction         : [%lf, %lf, %lf] \\n"
"force             : [%lf, %lf, %lf] N\\n"
"impulse           : [%lf, %lf, %lf] Ns\\n"
"force-magnitude   : %lf N\\n"
"impulse-magnitude : %lf Ns\\n"
"***************************************************************************\\n"
%(ventset_obj.direction[0], ventset_obj.direction[1], ventset_obj.direction[2],
  ventset_obj.force[0], ventset_obj.force[1], ventset_obj.force[2],
  ventset_obj.impulse[0], ventset_obj.impulse[1], ventset_obj.impulse[2],
  ventset_obj.force_magnitude,
  ventset_obj.impulse_magnitude))

ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\\n"
"***************************************************************************\\n"
"Post-init consolidated values:\\n"
"direction         : [%lf, %lf, %lf] \\n"
"force             : [%lf, %lf, %lf] N\\n"
"impulse           : [%lf, %lf, %lf] Ns\\n"
"force-magnitude   : %lf N\\n"
"impulse-magnitude : %lf Ns\\n"
"***************************************************************************\\n"
%(ventset_obj.direction[0], ventset_obj.direction[1], ventset_obj.direction[2],
  ventset_obj.force[0], ventset_obj.force[1], ventset_obj.force[2],
  ventset_obj.impulse[0], ventset_obj.impulse[1], ventset_obj.impulse[2],
  ventset_obj.force_magnitude,
  ventset_obj.impulse_magnitude))
""")
trick.stop(0)

