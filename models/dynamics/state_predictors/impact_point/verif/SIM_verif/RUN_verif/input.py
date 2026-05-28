dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4;
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_SinglePlanet
dynamics.dyn_manager_init.central_point_name = "Earth"


exec(open( "Log_data/log_state.py").read())

vehicle.dyn_body.set_name("vehicle")
vehicle.dyn_body.integ_frame_name = "Earth.inertial"
vehicle.dyn_body.translational_dynamics = True
vehicle.dyn_body.rotational_dynamics = False

vehicle.mass_init.set_subject_body( vehicle.dyn_body )
vehicle.mass_init.properties.pt_orientation.data_source = trick.Orientation.InputQuaternion
vehicle.mass_init.properties.mass  = 300.0
vehicle.mass_init.properties.position = [ 0.0, 0.0, 0.0]
vehicle.mass_init.properties.inertia = [[100.0,   0.0,   0.0],
                                        [  0.0, 200.0,   0.0],
                                        [  0.0,   0.0, 400.0]]

dynamics.dyn_manager.add_body_action(vehicle.mass_init)



vehicle.trans_init.set_subject_body( vehicle.dyn_body )
vehicle.trans_init.reference_ref_frame_name = "Earth.inertial"
vehicle.trans_init.body_frame_id = "composite_body"
vehicle.trans_init.position = [6.5E6, 0.0, 0.0]
vehicle.trans_init.velocity = [-10.0, 4000.0, 3000.0]

dynamics.dyn_manager.add_body_action (vehicle.trans_init)


vehicle.grav_control.source_name   = "Earth"
vehicle.grav_control.active        = True
vehicle.grav_control.spherical     = True

vehicle.dyn_body.grav_interaction.add_control(vehicle.grav_control)


impact_point.reference_position = [6.4E6, 0.0, 0.0]
impact_point.impact_point.subscribe()

impact_point.planet_rel_state.reference_name = "Earth"

trick.stop(215)
