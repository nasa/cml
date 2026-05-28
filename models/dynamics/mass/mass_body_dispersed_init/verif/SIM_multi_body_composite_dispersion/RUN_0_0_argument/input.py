# This run tests the case where composite-properties are set on body_0 by
# adjusting the core properties of the same body.
# The application is launched by using update with the specified adjustable-body
# in the argument.
# The results of this run should match those of the other 0_0 runs.
from Modified_data.setup import setup_0_0
setup_0_0(so, dynamics)
trick.add_read(0, "so.adjust_0.update( so.body[0])")
