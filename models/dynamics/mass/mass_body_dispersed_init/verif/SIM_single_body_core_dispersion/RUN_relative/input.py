from Modified_data.setup import setup_dispersions
setup_dispersions(mass_body_disp, dynamics)

mass_body_disp.mass_init.mass_disp_is_relative = True
mass_body_disp.mass_init.pos_disp_is_relative = True
mass_body_disp.mass_init.inertia_moi_disp_is_relative = True
mass_body_disp.mass_init.inertia_poi_disp_is_relative = True
