dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_SinglePlanet
dynamics.dyn_manager_init.central_point_name = "Earth"

# Switch off the update_before_deactivating flag; we know that the state
# is current when the unsubscribe command is issued so do not need to wait
# for a scheduled update.
vehicle.traj.update_before_deactivating = False
# Can set the immediate_activity_change flag because we have nothing
# processing at the derivative rate, and no extraordinary events scheduled.
vehicle.traj.immediate_activity_change = True

def veh_setup( body, name):
  body.integ_frame_name = "Earth.inertial"
  body.set_name(name)
  body.translational_dynamics = True
  body.rotational_dynamics = True

def mass_init( mass_init, body):
  mass_init.set_subject_body(body)
  mass_init.properties.mass = 1.0
  mass_init.properties.position  = [0,0,0]
  mass_init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
  mass_init.properties.inertia = [[1,0,0],[0,1,0],[0,0,1]]
  mass_init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
  mass_init.properties.pt_orientation.trans = [[1,0,0],[0,1,0],[0,0,1]]
  dynamics.dyn_manager.add_body_action( mass_init)

def state_init( body):
  vehicle.state_initialize.set_subject_body(body)
  vehicle.state_initialize.body_frame_id = "composite_body"
  vehicle.state_initialize.planet_name = "Earth"

  vehicle.state_initialize.position_input_data_type = trick.StateInitialize.Inertial
  vehicle.state_initialize.velocity_input_data_type = trick.StateInitialize.Inertial
  vehicle.state_initialize.attitude_input_data_type = trick.StateInitialize.EulerInertial_YPR
  vehicle.state_initialize.att_rate_input_data_type = trick.StateInitialize.BodyInertialRate
  vehicle.state_initialize.yaw = 0.0
  vehicle.state_initialize.pitch = 0.0
  vehicle.state_initialize.roll = 0.0
  vehicle.state_initialize.roll_rate_body = 1.0
  vehicle.state_initialize.pitch_rate_body = 0.0
  vehicle.state_initialize.yaw_rate_body = 0.0
  vehicle.state_initialize.position = [1.0, 0.0, 0.0]
  vehicle.state_initialize.velocity = [1.0, 0.0, 0.0]
  vehicle.state_initialize.active = True
  dynamics.dyn_manager.add_body_action( vehicle.state_initialize)
