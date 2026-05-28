exec(open("RUN_04e_velocity_effect_with_rot/input.py").read())
#Turn off velocity effect inclusion part-way through scenario
trick.add_read(5.0,"""
vehicle.test_dyn.set_include_velocity_effect(False)
""")
