# This run tests the case where composite-properties are set on body_1 by
# adjusting the core properties of body_2.
# Specifically, this tests confirms that composite-properties further up the tree
# (i.e. body_0, body_1's parent) are affected by manipulation further down the
# tree.


# For the composite-properties given below, the core-properties of body 2 should return:
# mass: 2
# position: [1, 1, 1]
# inertia = [ 2.0, 0.1, 0.1]
#           [ 0.1, 3.0, 0.1]
#           [ 0.1, 0.1, 4.0]

# To within numerical rounding at the limit of precision, the results from this test case
# should match those from RUN_0_2.

from Modified_data.setup import env_setup
env_setup(so, dynamics)
so.adjust_1.nominal.mass = 4
so.adjust_1.nominal.position = [ 1.0,  0.5,  0.5]
so.adjust_1.nominal.inertia = [[ 5.0,  0.1, -1.1],
                               [ 0.1,  8.0, -0.1],
                               [ -1.1, -0.1, 9.0]]
so.adjust_1.adjustable_body = so.body[2]
dynamics.dyn_manager.add_body_action( so.adjust_1)
