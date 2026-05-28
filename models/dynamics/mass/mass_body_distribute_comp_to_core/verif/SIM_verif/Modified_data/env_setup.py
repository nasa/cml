dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_EmptySpace
dynamics.dyn_manager_init.central_point_name = "Space"

for ii in range(3):
  so.body[ii].name = "body_"+str(ii)

  so.mass_init[ii].set_subject_body( so.body[ii] )
  so.mass_init[ii].properties.mass = 3.0-ii
  so.mass_init[ii].properties.position = [ii,0,0] # Centre of mass in the structural frame
  so.mass_init[ii].properties.inertia = [[3-ii,0,0],[0,4-ii,0],[0,0,5-ii]] # Inertia matrix in the core frame
  so.mass_init[ii].properties.pt_orientation.data_source = trick.Orientation.InputEigenRotation # Core frame is rotated from structural frame...
  so.mass_init[ii].properties.pt_orientation.eigen_angle = trick.attach_units("degree", 90*ii) # by this many degrees...
  so.mass_init[ii].properties.pt_orientation.eigen_axis = [0,0,1] # about this axis.
  dynamics.dyn_manager.add_body_action( so.mass_init[ii])


for ii in range(2):
  so.attach[ii].set_subject_body( so.body[ii+1] )
  so.attach[ii].set_parent_body( so.body[ii] )
  so.attach[ii].offset_pstr_cstr_pstr = [ii+1,0,0] # Vector from parent structural origin to child structural origin, coordinatized in parent structure frame
  so.attach[ii].pstr_cstr.data_source = trick.Orientation.InputMatrix
  so.attach[ii].pstr_cstr.trans = [[0,1,0],[-1,0,0],[0,0,1]] # Transformation matrix from parent structure frame to child structure frame
  so.attach[ii].active = True
  dynamics.dyn_manager.add_body_action( so.attach[ii])

so.new_properties.mass = 8.0
so.new_properties.position = [0,0,0]
so.new_properties.inertia  = [[25,0,0],[0,25,0],[0,0,25]]
