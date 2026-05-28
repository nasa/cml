import trick
from Log_data.log_data import log_default

def env_setup(mass_body_disp, dynamics):
  log_default()

  dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
  dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_EmptySpace
  dynamics.dyn_manager_init.central_point_name = "Space"

  mass_body_disp.body.integ_frame_name = "Space.inertial"
  mass_body_disp.body.set_name("test_vehicle")
  mass_body_disp.body.translational_dynamics = True
  mass_body_disp.body.rotational_dynamics = True

  # Mass
  mass_body_disp.mass_init.set_subject_body(mass_body_disp.body)
  mass_body_disp.mass_init.properties.mass = 2.0
  mass_body_disp.mass_init.properties.position  = [2,2,2]
  mass_body_disp.mass_init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
  mass_body_disp.mass_init.properties.inertia = [[2,2,2],[2,2,2],[2,2,2]]
  mass_body_disp.mass_init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
  mass_body_disp.mass_init.properties.pt_orientation.trans = [[1,0,0],[0,1,0],[0,0,1]]

  dynamics.dyn_manager.add_body_action( mass_body_disp.mass_init)

  trick.stop(0.0)

def setup_dispersions(mass_body_disp, dynamics):
  env_setup(mass_body_disp, dynamics)

  mass_body_disp.mass_init.disperse.mass = 0.1
  mass_body_disp.mass_init.disperse.position = [0.1 , -0.2, 0.3]
  mass_body_disp.mass_init.disperse.moi = [0.11 , -0.22, 0.33]
  mass_body_disp.mass_init.disperse.poi = [-0.12, 0.13, 0.23]
