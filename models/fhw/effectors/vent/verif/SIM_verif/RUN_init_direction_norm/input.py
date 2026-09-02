exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vent_params.py").read())
log_simple_params()

# section 4.2.5 -- initialization
# 1: (Omit for SimpleVent) If the vent has been assigned flow-rate AND exhaust-speed, these are used to
#    compute the force-magnitude.


ventset_obj.extra_vent.set_direction([1.0, 4.0, -8.0])

ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\n"
"***************************************************************************\n"
"Pre-init settings:\n"
f"direction         : [{ventset_obj.direction[0]:f}, {ventset_obj.direction[1]:f}, {ventset_obj.direction[2]:f}]\n"
"***************************************************************************\n")

trick.stop(0)
