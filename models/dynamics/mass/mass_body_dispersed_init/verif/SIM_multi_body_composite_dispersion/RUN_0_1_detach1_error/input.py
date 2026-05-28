# This run tests the case where composite-properties are set on body_0 by
# adjusting the core properties of body_1 and excluding the mass properties of
# body_1 and body_2.
# By detaching body_1 an error message should appear for detaching the adjustable
# body from the target body. Therefore, the setup and results should end up being
# the same as RUN_0_1.

from Modified_data.setup import setup_0_1
setup_0_1(so, dynamics)

so.adjust_0.detach_sub_tree = so.body[1]
