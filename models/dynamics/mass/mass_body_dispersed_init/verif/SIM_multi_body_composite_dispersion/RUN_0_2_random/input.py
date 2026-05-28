# This run continues with the scenario where composite-properties are set
# on body_0 by adjusting the core properties of body_2.
# In particular, this run tests the addition of randomly generated
# dispersions to the nominal settings.
from Modified_data.setup import setup_0_2
setup_0_2(so, dynamics)

so.adjust_0.disperse.mass_disp_mag = 0.1
so.adjust_0.disperse.position_disp_mag[0] = 0.1
so.adjust_0.disperse.position_disp_mag[1] = 0.1
so.adjust_0.disperse.position_disp_mag[2] = 0.1
so.adjust_0.disperse.moi_disp_mag[0] = 0.1
so.adjust_0.disperse.moi_disp_mag[1] = 0.1
so.adjust_0.disperse.moi_disp_mag[2] = 0.1
so.adjust_0.disperse.poi_disp_mag[0] = 0.1
so.adjust_0.disperse.poi_disp_mag[1] = 0.1
so.adjust_0.disperse.poi_disp_mag[2] = 0.1
so.adjust_0.disperse.seed_rand = 1
so.adjust_0.random_num_distribution = True
