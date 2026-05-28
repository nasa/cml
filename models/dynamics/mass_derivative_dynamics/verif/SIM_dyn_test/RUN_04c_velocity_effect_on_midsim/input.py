exec(open("RUN_04a_velocity_effect_baseline/input.py").read())

# Turn on velocity effect inclusion part-way through scenario
trick.add_read(10.0,"""
vehicle.test_dyn.set_include_velocity_effect(True)
""")
