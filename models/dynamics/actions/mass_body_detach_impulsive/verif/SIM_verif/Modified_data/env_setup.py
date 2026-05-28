dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_EmptySpace
dynamics.dyn_manager_init.central_point_name = "Space"

vehicle.bodyA.integ_frame_name = "Space.inertial"
vehicle.bodyA.set_name("vehicleA")
vehicle.bodyA.translational_dynamics = True
vehicle.bodyA.rotational_dynamics = True

vehicle.bodyB.integ_frame_name = "Space.inertial"
vehicle.bodyB.set_name("vehicleB")
vehicle.bodyB.translational_dynamics = True
vehicle.bodyB.rotational_dynamics = True

# Mass
def mass_init(init,veh):
  init.set_subject_body( veh )
  init.properties.mass = 1.0
  init.properties.position  = [0,0,0]
  init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
  init.properties.inertia = [[1,0,0],[0,1,0],[0,0,1]]
  init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
  init.properties.pt_orientation.trans = [[1,0,0],[0,1,0],[0,0,1]]
  init.allocate_points(1)
  init.get_mass_point(0).set_name(veh.name.get_name())
  init.get_mass_point(0).position = [0,0,0]
  init.get_mass_point(0).pt_orientation.data_source = trick.Orientation.InputEigenRotation
  dynamics.dyn_manager.add_body_action( init)

mass_init (vehicle.mass_init_A, vehicle.bodyA)
mass_init (vehicle.mass_init_B, vehicle.bodyB)

#State
vehicle.trans_init.set_subject_body( vehicle.bodyA )
vehicle.trans_init.body_frame_id = "composite_body"
vehicle.trans_init.reference_ref_frame_name = "Space.inertial"
vehicle.trans_init.position = [0.0, 0.0, 0.0]
vehicle.trans_init.velocity = [1.0, 2.0, 3.0]
dynamics.dyn_manager.add_body_action( vehicle.trans_init )

vehicle.rot_init.set_subject_body( vehicle.bodyA )
vehicle.rot_init.body_frame_id = "composite_body"
vehicle.rot_init.reference_ref_frame_name = "Space.inertial"
vehicle.rot_init.orientation.data_source  = trick.Orientation.InputEigenRotation
vehicle.rot_init.orientation.eigen_angle  = 0.0
vehicle.rot_init.orientation.eigen_axis   = [0.0,0.0,1.0]
vehicle.rot_init.ang_velocity             = [0.0, 0.0, 0.0]
dynamics.dyn_manager.add_body_action( vehicle.rot_init )

#Attach

vehicle.attach_B_to_A.set_parent_body( vehicle.bodyA )
vehicle.attach_B_to_A.set_subject_body( vehicle.bodyB )
vehicle.attach_B_to_A.subject_point_name = "vehicleB"
vehicle.attach_B_to_A.parent_point_name =  "vehicleA"
vehicle.attach_B_to_A.active = True

dynamics.dyn_manager.add_body_action( vehicle.attach_B_to_A )

#Detach
vehicle.detach_B_from_A.set_subject_body( vehicle.bodyB )
vehicle.detach_B_from_A.active = False
vehicle.detach_B_from_A.impulse_magnitude = 5.0
vehicle.detach_B_from_A.subject_point_name = "vehicleB"
vehicle.detach_B_from_A.parent_point_name =  "vehicleA"
dynamics.dyn_manager.add_body_action( vehicle.detach_B_from_A)
