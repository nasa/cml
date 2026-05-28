exec(open("RUN_06a_deactivate_baseline/input.py").read())

# Call unsubscribe() so that deactivate() is called
# Don't have set_include_velocity_effect() called yet so it skips that logic
trick.add_read(2.0,"""
vehicle.test_dyn.unsubscribe()
""")
