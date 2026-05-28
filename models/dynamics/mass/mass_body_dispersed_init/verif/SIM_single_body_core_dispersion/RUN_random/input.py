from Modified_data.setup import env_setup
env_setup(mass_body_disp, dynamics)

mass_body_disp.mass_init.disperse.mass_disp_mag = 0.5
mass_body_disp.mass_init.disperse.position_disp_mag[0] = 0.5
mass_body_disp.mass_init.disperse.position_disp_mag[1] = 0.5
mass_body_disp.mass_init.disperse.position_disp_mag[2] = 0.5
mass_body_disp.mass_init.disperse.moi_disp_mag[0] = 0.5
mass_body_disp.mass_init.disperse.moi_disp_mag[1] = 0.5
mass_body_disp.mass_init.disperse.moi_disp_mag[2] = 0.5
mass_body_disp.mass_init.disperse.poi_disp_mag[0] = 0.5
mass_body_disp.mass_init.disperse.poi_disp_mag[1] = 0.5
mass_body_disp.mass_init.disperse.poi_disp_mag[2] = 0.5

mass_body_disp.mass_init.disperse.seed_rand = 1

mass_body_disp.mass_init.random_num_distribution = True
