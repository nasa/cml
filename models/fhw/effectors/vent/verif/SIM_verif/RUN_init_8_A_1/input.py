exec(open("Modified_data/env_setup.py").read())

# section 4.2.5 -- initialization
# 8. (Omit for SimpleVent) Consolidate force-magnitude, flow-rate, and exhaust-speed
#    A. If force-magnitude and one of flow-rate or exhaust-speed were specified,
#       generate the missing variable.

# Test 1 flowrate is the missing variable in this equation.

exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vent_params.py").read())
log_full_params()

# For code-coverage, going to set flowrate to an invalid value, which results
# in zeroing the value and unsetting flowrate.
#    ventset_obj.extra_vent.unset_flowrate()
ventset_obj.extra_vent.set_flowrate(-1.0)
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



trick.add_read(0.0, '''
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\\n"
"***************************************************************************\\n"
"Post-init settings:\\n"
"force           : [%lf, %lf, %lf] Ns\\n"
"force-magnitude : %lf Ns\\n"
"flowrate        : %lf\\n"
"exhaust-speed   : %lf\\n"
"force-set       : %d\\n"
"force-mag-set   : %d\\n"
"flowrate-set    : %d\\n"
"exhaust-set     : %d\\n"
"***************************************************************************\\n"
%( ventset_obj.force[0], ventset_obj.force[1], ventset_obj.force[2],
  ventset_obj.force_magnitude,
  ventset_obj.flowrate,
  ventset_obj.exhaust_speed,
  ventset_obj.force_set,
  ventset_obj.force_magnitude_set,
  ventset_obj.flowrate_set,
  ventset_obj.exhaust_speed_set))
''')
trick.stop(0)
