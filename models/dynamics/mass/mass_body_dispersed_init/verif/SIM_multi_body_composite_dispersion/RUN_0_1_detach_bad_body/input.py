# This run tests the case where composite-properties are set on body_0 by
# adjusting the core properties of body_1 and excluding the mass properties of
# a completely separate body that isn't attached to the main mass tree.

# Since the body being deatched isn't part of the main mass tree this ends
# up being the same setup as RUN_0_1, producing the same results as the
# main mass tree isn't affected from the detach process.

from Modified_data.setup import setup_0_1
setup_0_1(so, dynamics)

so.adjust_0.detach_sub_tree = so.body[3]
