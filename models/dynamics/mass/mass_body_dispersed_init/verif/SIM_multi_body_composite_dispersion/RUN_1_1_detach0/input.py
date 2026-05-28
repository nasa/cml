# This run tests the case where composite-properties are set on body_1 by
# adjusting the core properties of the same body.
# The instruction is provided to detach body[0], but that is superior to
# the target-body, so these results should match those from RUN_1_1.
from Modified_data.setup import setup_1_1
setup_1_1(so, dynamics)
so.adjust_1.detach_sub_tree = so.body[0]
