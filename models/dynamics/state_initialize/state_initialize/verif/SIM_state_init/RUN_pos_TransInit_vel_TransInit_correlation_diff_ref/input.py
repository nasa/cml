exec(open("RUN_pos_TransInit_vel_TransInit/input.py").read())
exec(open("Modified_data/correlation_setup.py").read())

vehicle.state_initialize.correlation.corr_base_frame_name = 'Earth.pfix'
vehicle.state_initialize.trans_init.reference_ref_frame_name = 'Earth.inertial'
vehicle.state_initialize.rot_init.reference_ref_frame_name = 'Earth.inertial'

earth.planet.pfix.state.rot.T_parent_this[0][0] = 1.0
earth.planet.pfix.state.rot.T_parent_this[1][1] = -1.0
earth.planet.pfix.state.rot.T_parent_this[2][2] = -1.0

earth.planet.pfix.state.rot.Q_parent_this.scalar = 0.0
earth.planet.pfix.state.rot.Q_parent_this.vector[0] = 1.0
earth.planet.pfix.state.rot.Q_parent_this.vector[1] = 0.0
earth.planet.pfix.state.rot.Q_parent_this.vector[2] = 0.0