exec(open("Modified_data/env_setup.py").read())

ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\n"
"***************************************************************************\n"
"Unsetting duration by setting duration negative.\n"
"duration before : %lf\n"
"flag before     : %d\n"
%(ventset_obj.duration, ventset_obj.duration_set))
ventset_obj.extra_vent.set_duration(-1.0)
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print(
"duration after : %lf\n"
"flag after     : %d\n"
"***************************************************************************"
%(ventset_obj.duration, ventset_obj.duration_set))
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
"duration before: %lf"
%(ventset_obj.duration))

ventset_obj.extra_vent.set_force_vector([0,1,0], True)
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("duration after : %lf\\n"
"***************************************************************************"
%(ventset_obj.duration))
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
"       exhaust speed before: %lf\\n"
"***************************************************************************"
%(ventset_obj.exhaust_speed))

ventset_obj.extra_vent.set_exhaust_speed(-1.0)
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print(
"***************************************************************************\\n"
"       exhaust speed after : %lf\\n"
"***************************************************************************"
%(ventset_obj.exhaust_speed))
""")

trick.add_read(0,"""
ventset_obj.extra_vent.use_dynamic_mode()
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print("\\n\\n"
"***************************************************************************\\n"
"Error: Setting flowrate to invalid value:\\n"
"       flowrate before: %lf\\n"
"***************************************************************************"
%(ventset_obj.flowrate))

ventset_obj.extra_vent.set_flowrate(-1.0)
ventset_obj.copy_parameters(ventset_obj.extra_vent)
print(
"***************************************************************************\\n"
"       flowrate after : %lf\\n"
"***************************************************************************"
%(ventset_obj.flowrate))
""")
trick.stop(0)
