# This run tests the case where composite-properties are set on body-1 by
# adjusting the core properties of the same body.
# The instruction is provided to set the mass properties with body-2 detached.
# Body-2 is then re-attached so the end result for the composite-properties of
# body-1 should not match those specified.
# However, because the composite properties are set on body-1 while nothing is
# attached to it, body-1 should pick up core properties equal to those
# specified as its composite-properties.

# Thus we specify these values for composite-properties and expect them to copy
# into core-properties.
# mass:      1.5
# position: [1.1,  0.2,  0.3]
# inertia:  [ 2.1,  0.2, -0.3]
#           [ 0.2,  2.9,  0.1]
#           [-0.3,  0.1,  4.5]
from Modified_data.setup import env_setup
env_setup(so, dynamics)
so.adjust_1.detach_sub_tree = so.body[2]

so.adjust_1.nominal.mass =1.5
so.adjust_1.nominal.position = [ 1.1, 0.2,  0.3]
so.adjust_1.nominal.inertia = [[ 2.1, 0.2, -0.3],
                               [ 0.2, 2.9,  0.1],
                               [-0.3, 0.1,  4.5]]

so.adjust_1.adjustable_body = so.body[1]
dynamics.dyn_manager.add_body_action( so.adjust_1)
