exec(open("RUN_R3/input.py").read())
test.ref_position = [-8000, 5000, 5000]

# Add in call to compute_pos_vel_from_params() using a non-default value for the input_theta_type
# argument (see the value 1 as the second argument)
trick.add_read(2.5,"""
test.tr_params_to_R3.compute_pos_vel_from_params(test.params_out, 1, [10000,0,0], [0,10000,0])
""")
