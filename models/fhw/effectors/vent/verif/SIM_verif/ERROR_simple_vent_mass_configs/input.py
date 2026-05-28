exec(open("Modified_data/env_setup.py").read())

print("\n"
"**********************************************************************\n"
"Error (x2): SimpleVent has no flowrate -- setter and getter\n"
"**********************************************************************")
ventset_obj.extra_simple_vent.set_flowrate(1.0)
flowrate = ventset_obj.extra_simple_vent.get_flowrate()
print("\n"
"**********************************************************************\n"
"Error (x2): SimpleVent has no exhaust-speed -- setter and getter\n"
"**********************************************************************")
ventset_obj.extra_simple_vent.set_exhaust_speed(1.0)
exhaust = ventset_obj.extra_simple_vent.get_exhaust_speed()
trick.stop(0)
