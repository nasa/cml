exec(open("Modified_data/env_setup.py").read())

# section 4.2.5 -- initialization
# 2. The direction is generated
#   D. Otherwise, system is under-specified; initialization fails.

ventset_obj.extra_vent.unset_direction()

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

print("\n"
"****************************************************************************\n"
"Errors (x2)\n"
"- Vent initialization fails due to unspecified direction.\n"
"- Vent Set initialization fails due to uninitialized Vent. TERMINAL.\n"
"****************************************************************************")
