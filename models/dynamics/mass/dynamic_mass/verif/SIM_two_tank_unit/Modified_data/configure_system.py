mass_test.group.subscribe()

# if not using the external call to series_flow, use the string mechanism
# instead.
if (mass_test.use_series_flow_method == False) :
  mass_test.string.add_mass_to_string( mass_test.tank_a)
  mass_test.string.add_mass_to_string( mass_test.tank_b)
  mass_test.group.add_string_to_group( mass_test.string)
else :
  mass_test.group.add_mass_to_group( mass_test.tank_a)
  mass_test.group.add_mass_to_group( mass_test.tank_b)

mass_test.mass_init[0].set_subject_body( mass_test.tank_a )
mass_test.mass_init[1].set_subject_body( mass_test.body )
mass_test.mass_init[2].set_subject_body( mass_test.tank_b )
mass_test.mass_init[3].set_subject_body( mass_test.tank_c )
for ii in range (0,4):
  mass_test.mass_init[ii].properties.mass = 1000.0
  mass_test.mass_init[ii].properties.position = [ii-1, 0, 0]
  mass_test.mass_init[ii].properties.inertia =  [[1,0,0],[0,1,0],[0,0,1]]
  mass_test.mass_init[ii].properties.pt_orientation.data_source = trick.Orientation.InputMatrix
  mass_test.mass_init[ii].properties.pt_orientation.trans  =  [[1,0,0],[0,1,0],[0,0,1]]

  dynamics.dyn_manager.add_body_action( mass_test.mass_init[ii] )

mass_test.tank_attach[0].set_subject_body( mass_test.tank_a )
mass_test.tank_attach[1].set_subject_body( mass_test.tank_b )
mass_test.tank_attach[2].set_subject_body( mass_test.tank_c )
for ii in range (0,3):
  mass_test.tank_attach[ii].set_parent_body( mass_test.body )
  mass_test.tank_attach[ii].offset_pstr_cstr_pstr = [0.0,0.0,0.0]
  mass_test.tank_attach[ii].pstr_cstr.data_source = trick.Orientation.InputMatrix;
  mass_test.tank_attach[ii].pstr_cstr.trans  =  [[1,0,0],[0,1,0],[0,0,1]]
  dynamics.dyn_manager.add_body_action( mass_test.tank_attach[ii] );
mass_test.tank_attach[2].active = False

dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_EmptySpace
dynamics.dyn_manager_init.central_point_name = "Space"

mass_test.body.integ_frame_name = "Space.inertial"
mass_test.body.set_name("vehicle")
mass_test.body.translational_dynamics = False
mass_test.body.rotational_dynamics = False
