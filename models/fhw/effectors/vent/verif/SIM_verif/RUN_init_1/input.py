exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vent_params.py").read())
log_full_params()

# section 4.2.5 -- initialization
# 1: (Omit for SimpleVent) If the vent has been assigned flow-rate AND exhaust-speed, these are used to
#    compute the force-magnitude.



ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\n"
"***************************************************************************\n"
"Pre-init settings:\n"
f"flowrate          : {ventset_obj.flowrate:f} kg/s\n"
f"exhaust_speed     : {ventset_obj.exhaust_speed:f} m/s\n"
f"force magnitude   : {ventset_obj.force_magnitude:f} N\n"
f"force-mag-set     : {ventset_obj.force_magnitude_set:d}\n"
"***************************************************************************\n")


trick.add_read(0.0, '''
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\\n"
"***************************************************************************\n"
"Pre-init settings:\n"
f"flowrate          : {ventset_obj.flowrate:f} kg/s\n"
f"exhaust_speed     : {ventset_obj.exhaust_speed:f} m/s\n"
f"force magnitude   : {ventset_obj.force_magnitude:f} N\n"
f"force-mag-set     : {ventset_obj.force_magnitude_set:d}\n"
"***************************************************************************\n")


force = ventset_obj.force_magnitude
force_ext = ventset_obj.flowrate * ventset_obj.exhaust_speed
print('computed force_magnitude : %f N\\n'%force_ext)

# if the model-generated and externally-computed values are different, terminate
# the sim with non-ZERO return code
if force != force_ext:
  trick.exec_terminate_with_return(1, "RUN_init_1/input.py", 30,
                                   "computed force_magnitude [%f] is different than the model computed force_magnitude [%f]"
                                   %(force_ext, force))
print("Section 4.2.5, rule 1, verification passed!\\n")
''')
trick.stop(0)
