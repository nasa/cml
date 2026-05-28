# This run replicates RUN_absolute, but sets the poi dispersion to represent
# positive integrals. The off diagonal elements of the inertia tensor should
# shift away from the RUN_nominal values in the opposite direction to that
# observed in the RUN_absolute results.
from Modified_data.setup import setup_dispersions
setup_dispersions(mass_body_disp, dynamics)
mass_body_disp.mass_init.poi_are_negative_integrals = False
