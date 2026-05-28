# section 4.2.5 -- initialization
# 6. The force vector is generated
#    A. If force-magnitude is available, it is used with the direction to
#       generate the force-vector
#       i. The force-magnitude and direction vector are combined to generate 
#          the force-vector, overriding any assigned force-vector if the 
#          specified direction is inconsistent with the force-vector direction

# Test2 -- force vector also specified, overconstrained.

exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vent_params.py").read())
log_full_params()

ventset_obj.extra_vent.set_force_vector([1.0, 4.0, 8.0])
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\n"
"***************************************************************************\n"
"Pre-init settings:\n"
"direction         : [%lf, %lf, %lf] \n"
"force             : [%lf, %lf, %lf] N\n"
"direction-set     : %d\n"
"force-set         : %d\n"
"***************************************************************************\n"
%(ventset_obj.direction[0], ventset_obj.direction[1], ventset_obj.direction[2],
  ventset_obj.force[0], ventset_obj.force[1], ventset_obj.force[2],
  ventset_obj.direction_set,
  ventset_obj.force_set))

print("\n"
"*********************************************************\n"
"Warning messages expected:\n"
" - force-magnitude of \"extra_vent\" is over-constrained.\n"
" - direction of \"extra_vent\" is over-constrained.\n"
" - force-vector of \"extra_vent\" is over-constrained.\n"
"*********************************************************")


trick.add_read(0.0, '''
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print(
"*********************************************************\\n"
"Return to test\\n"
"*********************************************************")


print("\\n"
"***************************************************************************\\n"
"Post-init settings:\\n"
"direction         : [%lf, %lf, %lf] \\n"
"force             : [%lf, %lf, %lf] N\\n"
"direction-set     : %d\\n"
"force-set         : %d\\n"
"***************************************************************************\\n"
%(ventset_obj.direction[0], ventset_obj.direction[1], ventset_obj.direction[2],
  ventset_obj.force[0], ventset_obj.force[1], ventset_obj.force[2],
  ventset_obj.direction_set,
  ventset_obj.force_set))
''')
trick.stop(0)
