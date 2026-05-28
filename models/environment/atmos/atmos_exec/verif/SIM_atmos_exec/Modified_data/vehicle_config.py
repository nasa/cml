def veh_config( veh, name) :
  veh.body.integ_frame_name = "Earth.inertial"
  veh.body.set_name(name)
  veh.body.translational_dynamics = True
  veh.body.rotational_dynamics = True
  
  veh.planet_rel_state.reference_name = "Earth"
  
  # Mass
  veh.mass_init.set_subject_body(veh.body)
  veh.mass_init.properties.mass = 1.0
  veh.mass_init.properties.position  = [0,0,0]
  veh.mass_init.properties.inertia_spec = trick.MassPropertiesInit.StructCG
  veh.mass_init.properties.inertia = [[1,0,0],[0,1,0],[0,0,1]]
  veh.mass_init.properties.pt_orientation.data_source = trick.Orientation.InputMatrix
  veh.mass_init.properties.pt_orientation.trans = [[1,0,0],[0,1,0],[0,0,1]]

  dynamics.dyn_manager.add_body_action( veh.mass_init)

def root_veh_config( veh, name) :
  veh_config( veh, name)
  # Gravity
  veh.grav_controls_earth.source_name = "Earth"
  veh.grav_controls_earth.active = True
  veh.grav_controls_earth.spherical = True
  veh.grav_controls_earth.gradient = False
  veh.body.add_control( veh.grav_controls_earth )
  
  #State
  veh.state_init.set_subject_body(veh.body)
  veh.state_init.body_frame_id = "composite_body"
  veh.state_init.planet_name = "Earth"
  
  veh.state_init.position_input_data_type = trick.StateInitialize.Inertial
  veh.state_init.velocity_input_data_type = trick.StateInitialize.Inertial
  veh.state_init.attitude_input_data_type = trick.StateInitialize.EulerInertial_YPR
  veh.state_init.att_rate_input_data_type = trick.StateInitialize.BodyInertialRate
  veh.state_init.yaw = 0.0
  veh.state_init.pitch = 0.0
  veh.state_init.roll = 0.0
  veh.state_init.roll_rate_body = 0.0
  veh.state_init.pitch_rate_body = 0.0
  veh.state_init.yaw_rate_body = 0.0
  veh.state_init.position = [6500000.0, 0.0, 0.0]
  veh.state_init.velocity = [0.0, 8000.0, 0.0]
  veh.state_init.active = True
  dynamics.dyn_manager.add_body_action( veh.state_init)

def sub_veh_config( veh, parent, name) :
  veh_config( veh, name)
  # attach
  veh.body_attach.action_name = "attach "+name
  veh.body_attach.set_subject_body(veh.body)
  veh.body_attach.set_parent_body(parent.body)
  veh.body_attach.offset_pstr_cstr_pstr = [0,0,0]
  veh.body_attach.pstr_cstr.data_source = trick.Orientation.InputMatrix
  veh.body_attach.pstr_cstr.trans = [[1,0,0],[0,1,0],[0,0,1]]
  veh.body_attach.active = True
  dynamics.dyn_manager.add_body_action( veh.body_attach)
