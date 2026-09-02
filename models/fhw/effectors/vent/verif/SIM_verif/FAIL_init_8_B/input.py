exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vent_params.py").read())

# section 4.2.5 -- initialization
# 8. (Omit for SimpleVent) Consolidate force-magnitude, flow-rate, and exhaust-speed
#    B. If in impulse-mode and exhaust-speed is not set (after attempts in step 8-A), 
#       the system is under-specified; initialization fails

log_simple_params()

ventset_obj.extra_vent.unset_exhaust_speed()
ventset_obj.extra_vent.unset_flowrate()
ventset_obj.extra_vent.set_force_magnitude(5)

ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\n"
"***************************************************************************\n"
"Pre-init settings:\n"
f"force           : [{ventset_obj.force[0]:f}, {ventset_obj.force[1]:f}, {ventset_obj.force[2]:f}] N\n"
f"force-magnitude : {ventset_obj.force_magnitude:f}\n"
f"flowrate        : {ventset_obj.flowrate:f}\n"
f"exhaust-speed   : {ventset_obj.exhaust_speed:f}\n"
f"force-set       : {ventset_obj.force_set:d}\n"
f"force-mag-set   : {ventset_obj.force_magnitude_set:d}\n"
f"flowrate-set    : {ventset_obj.flowrate_set:d}\n"
f"exhaust-set     : {ventset_obj.exhaust_speed_set:d}\n"
"***************************************************************************\n")

print("\n"
"****************************************************************************\n"
"Errors (x2)\n"
"- Vent initialization fails due to unspecified exhaust-speed.\n"
"- Vent Set initialization fails due to uninitialized Vent. TERMINAL.\n"
"****************************************************************************")
