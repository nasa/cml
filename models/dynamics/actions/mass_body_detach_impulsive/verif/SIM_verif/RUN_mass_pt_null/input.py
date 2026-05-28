exec(open("RUN_central_impulse/input.py").read())

# Added a null parent mass pt in order to hit the NULL check in
# apply_detach().
vehicle.detach_B_from_A.terminate_on_error = False

trick.add_read(3.0,"""
vehicle.detach_B_from_A.parent_point_name = "Spaghetti"
""")
