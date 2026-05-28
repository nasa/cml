exec(open("Modified_data/env_setup.py").read())


print("\n"
"**********************************************************************\n"
"Error (x2)\n"
" - In Matrix3x3::invert_sym, problem inverting inertia matrix.\n"
" - In vent-set, cannot apply delta-omega without inverse intertia.\n"
"**********************************************************************")

ventset_obj.set_2_tanks.get_vent(0).set_duration(1.0)

# Trigger the "Invalid inertia" error in VentSet::apply_impulse_to_body
trick.add_read(0, """
ventset_obj.root_body.composite_properties.inertia[0][0] = 0.0
ventset_obj.set_2_tanks.start_vent(1)
""")
trick.stop(0)
