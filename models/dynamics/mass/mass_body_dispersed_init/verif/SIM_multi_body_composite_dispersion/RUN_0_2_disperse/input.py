# This run continues with the scenario where composite-properties are set
# on body_0 by adjusting the core properties of body_2.
# In particular, this run tests the addition of a specified dispersion to
# the nominal settings to verify that dispersions are propagated onto the
# target body.
from Modified_data.setup import setup_0_2
setup_0_2(so, dynamics)

so.adjust_0.disperse.mass        = so.adjust_0.nominal.mass          *  0.1
so.adjust_0.disperse.position[0] = so.adjust_0.nominal.position[0]   *  0.1
so.adjust_0.disperse.position[1] = so.adjust_0.nominal.position[1]   * -0.1
so.adjust_0.disperse.position[2] = so.adjust_0.nominal.position[2]   *  0.2
so.adjust_0.disperse.moi[0]      = so.adjust_0.nominal.inertia[0][0] *  0.1
so.adjust_0.disperse.moi[1]      = so.adjust_0.nominal.inertia[1][1] *  0.15
so.adjust_0.disperse.moi[2]      = so.adjust_0.nominal.inertia[2][2] *  0.2
so.adjust_0.disperse.poi[0]      = so.adjust_0.nominal.inertia[0][1] * -0.1
so.adjust_0.disperse.poi[1]      = so.adjust_0.nominal.inertia[0][2] * -0.15
so.adjust_0.disperse.poi[2]      = so.adjust_0.nominal.inertia[1][2] * -0.2
