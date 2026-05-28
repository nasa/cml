# This run tests the case where composite-properties are set on body_0 by
# adjusting the core properties of the same body.
# The application is launched from the DynManager by adding to the available
# BodyAction instances.
# The results of this run should match those of the other 0_0 runs.
from Modified_data.setup import setup_0_0
setup_0_0(so, dynamics)
so.adjust_0.adjustable_body = so.body[0]
dynamics.dyn_manager.add_body_action( so.adjust_0)
