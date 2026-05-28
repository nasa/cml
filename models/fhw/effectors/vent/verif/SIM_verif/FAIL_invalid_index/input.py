exec(open("Modified_data/env_setup.py").read())


print("\n"
"**********************************************************************\n"
"Terminal Error -- index out of domain\n"
"**********************************************************************")

vent = ventset_obj.set_simple.get_vent(999)
