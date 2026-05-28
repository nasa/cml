# This run tests the case where composite-properties are set on body_0 by
# adjusting the core properties of body_1 and excluding the mass properties of
# body_2.
# Body_2 is then re-attached so the end result for the composite-properties of
# body_0 should not match those specified. However, they should match with the
# results from RUN_0_1 and the core-properties of body_1
# should also still match those from RUN_0_1:

from Modified_data.setup import env_setup
env_setup(so, dynamics)

so.adjust_0.nominal.mass = 4.5
so.adjust_0.nominal.position = [ 0.2666666666666667,  0.3666666666666667, 0.1]
so.adjust_0.nominal.inertia = [[ 6.4,  -0.68,  0.06],
                               [-0.68,  7.63, -0.43],
                               [ 0.06, -0.43, 11.35]]

so.adjust_0.adjustable_body = so.body[1]
dynamics.dyn_manager.add_body_action( so.adjust_0)

so.adjust_0.detach_sub_tree = so.body[2]
