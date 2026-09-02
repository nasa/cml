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
f"direction         : [{ventset_obj.direction[0]:f}, {ventset_obj.direction[1]:f}, {ventset_obj.direction[2]:f}] \n"
f"force             : [{ventset_obj.force[0]:f}, {ventset_obj.force[1]:f}, {ventset_obj.force[2]:f}] N\n"
f"impulse           : [{ventset_obj.impulse[0]:f}, {ventset_obj.impulse[1]:f}, {ventset_obj.impulse[2]:f}] N\n"
f"direction-set     : {ventset_obj.direction_set:d}\n"
f"force-set         : {ventset_obj.force_set:d}\n"
"***************************************************************************\n")

print("\n"
"****************************************************************************\n"
"Errors (x2)\n"
"- Vent initialization fails due to unspecified impulse.\n"
"- Vent Set initialization fails due to uninitialized Vent. TERMINAL.\n"
"****************************************************************************")
