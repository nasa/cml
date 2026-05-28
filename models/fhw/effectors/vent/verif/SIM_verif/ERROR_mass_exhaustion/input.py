exec(open("Modified_data/env_setup.py").read())
ventset_obj.set_2_tanks.start_vents_at_activation = True


ventset_obj.set_2_tanks.get_vent(0).set_duration(2)

print("\n"
"**********************************************************************\n"
"Error (x3):\n"
"  - cannot vent from an empty tank\n"
"  - cannot complete an impulsive vent from a tank with\n"
"    insufficient propellant.\n"
"**********************************************************************")
# Trigger the "Empty tank" warning in Vent::start_venting
ventset_obj.tank[1].dynamic_properties.consumable_mass = 0.0

# Trigger the "Not enough mass in tank for requested impulse" warning in
# Vent::check_status
ventset_obj.extra_vent.set_impulse_magnitude(10000.0)
trick.stop(0)
