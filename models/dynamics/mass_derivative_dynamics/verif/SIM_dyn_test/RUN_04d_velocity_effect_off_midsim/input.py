exec(open("RUN_04b_velocity_effect_on/input.py").read())

#Turn off velocity effect inclusion part-way through scenario
trick.add_read(15.0,"""
vehicle.test_dyn.set_include_velocity_effect(False)
""")
