exec(open("Modified_data/env_setup.py").read())

ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\n"
"***************************************************************************\n"
"Unsetting duration by setting duration negative.\n"
f"duration before : {ventset_obj.duration:f}\n"
f"flag before     : {ventset_obj.duration_set:d}\n")
ventset_obj.extra_vent.set_duration(-1.0)
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print(
f"duration after : {ventset_obj.duration:f}\n"
f"flag after     : {ventset_obj.duration_set:d}\n"
"***************************************************************************")
# reset:
ventset_obj.extra_vent.set_duration(1.0)


trick.add_read(0,"""
print("\\n\\n"
"***************************************************************************\\n"
"Warning setting force to [0,0,0]\\n"
"***************************************************************************")
ventset_obj.extra_vent.set_force_vector([0,0,0])
""")


trick.add_read(0,"""
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\\n\\n"
"***************************************************************************\\n"
"Setting duration by setting force vector:\\n"
f"duration before: {ventset_obj.duration:f}")

ventset_obj.extra_vent.set_force_vector([0,1,0], True)
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print(f"duration after : {ventset_obj.duration:f}\\n"
"***************************************************************************")
""")




trick.add_read(0,"""
print("\\n\\n"
"***************************************************************************\\n"
"Warning setting impulse to [0,0,0]\\n"
"***************************************************************************")
ventset_obj.extra_vent.set_impulse_vector([0,0,0])
""")






trick.add_read(0,"""
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\\n\\n"
"***************************************************************************\\n"
"Error: Setting exhaust-speed to invalid value:\\n"
f"       exhaust speed before: {ventset_obj.exhaust_speed:f}\\n"
"***************************************************************************")

ventset_obj.extra_vent.set_exhaust_speed(-1.0)
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print(
"***************************************************************************\\n"
f"       exhaust speed after : {ventset_obj.exhaust_speed:f}\\n"
"***************************************************************************")
""")

trick.add_read(0,"""
ventset_obj.extra_vent.use_dynamic_mode()
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\\n\\n"
"***************************************************************************\\n"
"Error: Setting flowrate to invalid value:\\n"
f"       flowrate before: {ventset_obj.flowrate:f}\\n"
"***************************************************************************")

ventset_obj.extra_vent.set_flowrate(-1.0)
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print(
"***************************************************************************\\n"
f"       flowrate after : {ventset_obj.flowrate:f}\\n"
"***************************************************************************")
""")
trick.stop(0)
