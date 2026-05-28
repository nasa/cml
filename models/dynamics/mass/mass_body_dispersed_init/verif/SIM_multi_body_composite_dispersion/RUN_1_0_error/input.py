# This run tests the case where composite-properties are set on body_1 by
# adjusting the core properties of body_0.
# Because body_1 is a child of body_0, adjusting properties of body_0 cannot
# affect composite-properties of body_1.  This should result in an error
# message, abort of the process, and data that matches the nominal case.
from Modified_data.setup import setup_0_0
setup_0_0(so, dynamics)
so.adjust_1.adjustable_body = so.body[0]
dynamics.dyn_manager.add_body_action( so.adjust_1)
print( '\n'
'*************************************************************************\n'
'ERROR: invalid mass tree\n'
'attempt to adjust core properties of a superior body to\n'
'           target composite properties of an inferior body\n'
'*************************************************************************')
