# This run continues with the scenario where composite-properties are set
# on body_0 by adjusting the core properties of body_2.
# In particular, this run tests the addition of a specified dispersion to
# the nominal settings, where the dispersions are specified as fractions of the
# nominal.
# These results should match those of RUN_0_2_disperse
from Modified_data.setup import setup_0_2
setup_0_2(so, dynamics)

so.adjust_0.disperse.mass        =   0.1
so.adjust_0.disperse.position[0] =   0.1
so.adjust_0.disperse.position[1] =  -0.1
so.adjust_0.disperse.position[2] =   0.2
so.adjust_0.disperse.moi[0]      =   0.1
so.adjust_0.disperse.moi[1]      =   0.15
so.adjust_0.disperse.moi[2]      =   0.2
so.adjust_0.disperse.poi[0]      =  -0.1
so.adjust_0.disperse.poi[1]      =  -0.15
so.adjust_0.disperse.poi[2]      =  -0.2

so.adjust_0.mass_disp_is_relative = True
so.adjust_0.pos_disp_is_relative = True
so.adjust_0.inertia_moi_disp_is_relative = True
so.adjust_0.inertia_poi_disp_is_relative = True
