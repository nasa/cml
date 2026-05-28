# This run tests the case where composite-properties are set on body_0 by
# adjusting the core properties of body_2.
# Specifically, this tests the ability to navigate the mass tree beyond a
# simple parent-child relation.

# For the composite-properties given below, the core-properties of body 2 should return:
# mass: 2
# position: [1, 1, 1]
# inertia = [ 2.0, 0.1, 0.1]
#           [ 0.1, 3.0, 0.1]
#           [ 0.1, 0.1, 4.0]
from Modified_data.setup import setup_0_2
setup_0_2(so, dynamics)
