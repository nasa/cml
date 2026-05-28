# section 4.2.5 -- initialization
# 7. The impulse vector is generated as needed:
#   B. Otherwise (i.e. impulse-magnitude not known), then:
#     i. If in impulse mode, the system is under-specified and initialization fails.
exec(open("Modified_data/env_setup.py").read())

# remove information about impulse by:
# unset duration to remove F * dt option
ventset_obj.extra_vent.unset_duration()

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

print("\n"
"****************************************************************************\n"
"Errors (x2)\n"
"- Vent initialization fails due to unspecified impulse.\n"
"- Vent Set initialization fails due to uninitialized Vent. TERMINAL.\n"
"****************************************************************************")
