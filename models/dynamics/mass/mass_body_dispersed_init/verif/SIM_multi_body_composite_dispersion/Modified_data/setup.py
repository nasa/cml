import trick
from Log_data.log_data import log_default

def env_setup_no_log(so, dynamics):
  dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
  dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_EmptySpace
  dynamics.dyn_manager_init.central_point_name = "Space"

  for ii in range(3):
    so.body[ii].set_name("body_"+str(ii))

    so.mass_init[ii].set_subject_body( so.body[ii] )
    so.mass_init[ii].properties.mass = 3.0-ii
    so.mass_init[ii].properties.position  = [ii,0,0]
    so.mass_init[ii].properties.inertia = [[3-ii,0,0],[0,4-ii,0],[0,0,5-ii]]
    so.mass_init[ii].properties.pt_orientation.data_source = trick.Orientation.InputEigenRotation
    so.mass_init[ii].properties.pt_orientation.eigen_angle = trick.attach_units("degree", 90*ii)
    so.mass_init[ii].properties.pt_orientation.eigen_axis = [0,0,1]
    dynamics.dyn_manager.add_body_action( so.mass_init[ii])

  for ii in range(2):
    so.attach[ii].set_subject_body( so.body[ii+1] )
    so.attach[ii].set_parent_body( so.body[ii] )
    so.attach[ii].offset_pstr_cstr_pstr = [ii+1,0,0]
    so.attach[ii].pstr_cstr.data_source = trick.Orientation.InputMatrix
    so.attach[ii].pstr_cstr.trans = [[0,1,0],[-1,0,0],[0,0,1]]
    so.attach[ii].active = True
    dynamics.dyn_manager.add_body_action( so.attach[ii])

  so.adjust_0.active = True
  so.adjust_1.active = True

  trick.stop(0)

def env_setup(so, dynamics):
  env_setup_no_log(so, dynamics)
  log_default()


def setup_0_0(so, dynamics):
  env_setup(so, dynamics)
  # For all RUN_0_0, the composite-propperties specified below should generate
  # core-properties of body 0 of:
  # mass:       3.5
  # position: [ 1.1,  1.2,  1.3]
  # inertia:  [ 3.1,  0.2, -0.3]
  #           [ 0.2,  3.9,  0.1]
  #           [-0.3,  0.1,  5.5]
  so.adjust_0.nominal.mass = 6.5
  so.adjust_0.nominal.position = [ 0.7461538461538463,   1.261538461538461,  0.7000000000000001]
  so.adjust_0.nominal.inertia = [[ 9.525384615384617,    1.69846153846154,    -1.91],
                                 [ 1.69846153846154,    15.24615384615385,     0.3799999999999999],
                                 [-1.91,                 0.3799999999999999,  16.8115384615384]]

def setup_0_1(so, dynamics):
  env_setup(so, dynamics)
  # For the composite-properties given below, the core-properties of body 1 should return:
  # mass:      1.5
  # position: [1.1,  0.2,  0.3]
  # inertia:  [ 2.1,  0.2, -0.3]
  #           [ 0.2,  2.9,  0.1]
  #           [-0.3,  0.1,  4.5]
  so.adjust_0.nominal.mass = 5.5
  so.adjust_0.nominal.position = [0.0363636363636364,  0.6636363636363637, 0.08181818181818182]
  so.adjust_0.nominal.inertia = [[ 9.590909090909093,    1.012727272727274,  -0.04363636363636365],
                                 [ 1.012727272727274,   10.95090909090909,   -0.2963636363636365],
                                 [-0.04363636363636365, -0.2963636363636365, 17.84545454545454]]

  so.adjust_0.adjustable_body = so.body[1]
  dynamics.dyn_manager.add_body_action( so.adjust_0)

def setup_0_2(so, dynamics):
  env_setup(so, dynamics)

  so.adjust_0.nominal.mass = 7
  so.adjust_0.nominal.position = [ 0.2857142857142857,  0.5714285714285714,  0.2857142857142857]
  so.adjust_0.nominal.inertia = [[ 10.14285714285714,   -0.7571428571428559,  0.6714285714285715],
                                 [ -0.7571428571428559, 12.85714285714286,   -0.7571428571428577],
                                 [  0.6714285714285715, -0.7571428571428577, 16.14285714285714]]
  so.adjust_0.adjustable_body = so.body[2]
  dynamics.dyn_manager.add_body_action( so.adjust_0)


def setup_1_1(so, dynamics):
  env_setup(so, dynamics)

  so.adjust_1.nominal.mass = 2.5
  so.adjust_1.nominal.position = [ 1.46,   0.92,   0.18]
  so.adjust_1.nominal.inertia = [[ 3.64,   1.172,  0.024],
                                 [ 1.172,  6.898, -0.062],
                                 [ 0.024, -0.062,  9.93]]

  so.adjust_1.adjustable_body = so.body[1]
  dynamics.dyn_manager.add_body_action( so.adjust_1)
