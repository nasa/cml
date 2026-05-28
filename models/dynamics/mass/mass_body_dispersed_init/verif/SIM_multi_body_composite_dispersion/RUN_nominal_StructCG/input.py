# This run provides the nominal mass properties with no adjustment, with the
# specified inertia matrices being interpreted using basis vectors aligned
# with the structure frame. RUN_nominal has the inertia interpreted with basis
# vectors aligned with the body frame.
from Modified_data.setup import env_setup
env_setup(so, dynamics)
for ii in range(3):
  so.mass_init[ii].properties.inertia_spec = trick.MassPropertiesInit.StructCG
