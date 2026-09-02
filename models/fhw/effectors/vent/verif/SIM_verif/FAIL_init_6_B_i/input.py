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
f"direction         : [{ventset_obj.direction[0]:f}, {ventset_obj.direction[1]:f}, {ventset_obj.direction[2]:f}] \n"
f"force             : [{ventset_obj.force[0]:f}, {ventset_obj.force[1]:f}, {ventset_obj.force[2]:f}] N\n"
f"impulse           : [{ventset_obj.impulse[0]:f}, {ventset_obj.impulse[1]:f}, {ventset_obj.impulse[2]:f}] N\n"
f"direction-set     : {ventset_obj.direction_set:d}\n"
f"force-set         : {ventset_obj.force_set:d}\n"
"***************************************************************************\n")

print("\n"
"****************************************************************************\n"
"Errors (x2)\n"
"- Vent initialization fails due to unspecified force.\n"
"- Vent Set initialization fails due to uninitialized Vent. TERMINAL.\n"
"****************************************************************************")
