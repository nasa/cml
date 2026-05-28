# This run tests the case where composite-properties are set on body_1 by
# adjusting the core properties of the same body.
# This is used as a comparison for the test of setting the same values on a
# partial mass-tree in RUN_1_1_detach0 and RUN_1_1_detach2

# The composite-properties specified in setup_1_1 should generate
# the following core-properties for body-1:
# mass:      1.5
# position: [1.1,  0.2,  0.3]
# inertia:  [ 2.1,  0.2, -0.3]
#           [ 0.2,  2.9,  0.1]
#           [-0.3,  0.1,  4.5]
from Modified_data.setup import setup_1_1
setup_1_1(so, dynamics)
