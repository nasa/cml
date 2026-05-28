# This run tests the application of the properties change to a situation in
# which the initial inertia was specified as StructCG.
# NOTE - the dispersion of mass properties to meet composite-properties
#  only modifies the properties so that the target is reached. How the
#  initial mass properties are specified is irrelevant. Even though the
#  initial mass properties are specified as StructCG, the composite will still
#  be interpreted as Body and the values specified as adjust_0.nominal will
#  also still be interpreted as Body.

#  Expected effects of changing the basis used for specifying the inertia
#  tensors to mass-init:
#  - body[0] compoiste-properties:
#      no effect. These properties are the specified target values and the way
#      in which the inertia is specified to mass-init has absolutely no bearing;
#      the target values use body-axes regardles.
#  - body[0] core-properties:
#      no effect. The structure-point and body-point of body[0] are aligned and
#      coincident so specifying inertia using structure vs body frame bases are
#      equivalent processed.
#  - body[1] composite-properties.
#      no effect. The composite-properties of body[0] are generated from the
#      core-properties of body[0] and the composite-properties of body[1].
#      Because neither core nor composite change in body[0], the composite-
#      properties of body[1] must also not change.
# - Body[1] core-properties:
#      Affected. The composite-to-core adjustment does not affect body[0]
#      (body[2] is the adjustable body) so the core-inertia of body[1] is
#      defined entirely via mass-init. Body[1] has relative alignment between
#      the body and structure axes so changing the basis specification changes
#      the inertia tensor, and that tensor is retained.
#  - Body[2] core / composite-properties
#      Affected.  The composite-properties of body[1] are generated from the
#      core-properties of body[1] and the properties of body[2].  Because the
#      body[1] composite remains unchanged but the body[1] core does change,
#      the body[2] properties must also change to balance the change in body[1]
#      core properties.

from Modified_data.setup import setup_0_2
setup_0_2(so, dynamics)
for ii in range(3):
  so.mass_init[ii].properties.inertia_spec = trick.MassPropertiesInit.StructCG
