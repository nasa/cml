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
"force           : [%lf, %lf, %lf] Ns\n"
"force-magnitude : %lf Ns\n"
"flowrate        : %lf\n"
"exhaust-speed   : %lf\n"
"force-set       : %d\n"
"force-mag-set   : %d\n"
"flowrate-set    : %d\n"
"exhaust-set     : %d\n"
"***************************************************************************\n"
%( ventset_obj.force[0], ventset_obj.force[1], ventset_obj.force[2],
  ventset_obj.force_magnitude,
  ventset_obj.flowrate,
  ventset_obj.exhaust_speed,
  ventset_obj.force_set,
  ventset_obj.force_magnitude_set,
  ventset_obj.flowrate_set,
  ventset_obj.exhaust_speed_set))

print("\n"
"****************************************************************************\n"
"Errors (x2)\n"
"- Vent initialization fails due to unspecified exhaust-speed.\n"
"- Vent Set initialization fails due to uninitialized Vent. TERMINAL.\n"
"****************************************************************************")
