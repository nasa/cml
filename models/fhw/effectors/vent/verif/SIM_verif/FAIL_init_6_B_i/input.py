# section 4.2.5 -- initialization
# 6. The force vector is generated as needed:
#   B. Otherwise (i.e. force-magnitude not known), then:
#      i. If in dynamic mode, the system is under-specified; initialization fails.
exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vent_params.py").read())
log_simple_params()

# remove information about force by:
# unset flowrate to remove m-dot * v_e option
# unset duration to remove I / dt option
ventset_obj.extra_vent.unset_flowrate()
ventset_obj.extra_vent.unset_duration()
# set impulse instead of force:
ventset_obj.extra_vent.set_impulse_magnitude(9)
# Run in dynamic mode
ventset_obj.extra_vent.use_dynamic_mode()

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
"- Vent initialization fails due to unspecified force.\n"
"- Vent Set initialization fails due to uninitialized Vent. TERMINAL.\n"
"****************************************************************************")
