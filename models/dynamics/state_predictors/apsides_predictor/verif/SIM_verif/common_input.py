dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4;

jeod_time.time_manager_init.initializer = "UTC"
jeod_time.time_manager_init.sim_start_format = trick.TimeEnum.calendar
jeod_time.time_utc.calendar_year = 2013
jeod_time.time_utc.calendar_month = 10
jeod_time.time_utc.calendar_day = 17
jeod_time.time_utc.calendar_hour = 12
jeod_time.time_utc.calendar_minute = 00
jeod_time.time_utc.calendar_second = 0.0


vehicle.dyn_body.set_name("vehicle");
vehicle.dyn_body.integ_frame_name = "Earth.inertial"
vehicle.dyn_body.translational_dynamics = True
vehicle.dyn_body.rotational_dynamics = False

vehicle.mass_init.set_subject_body(vehicle.dyn_body)
vehicle.mass_init.properties.pt_orientation.data_source = trick.Orientation.InputQuaternion
# Orientation defaults to the identity quaternion
vehicle.mass_init.properties.mass = 300
vehicle.mass_init.properties.position = [ 0.0, 0.0, 0.0]
vehicle.mass_init.properties.inertia = [[ 100.0,   0.0,   0.0],
                                        [   0.0, 200.0,   0.0],
                                        [   0.0,   0.0, 400.0]]

dynamics.dyn_manager.add_body_action (vehicle.mass_init)



vehicle.trans_init.set_subject_body(vehicle.dyn_body)
vehicle.trans_init.reference_ref_frame_name = "Earth.inertial"
vehicle.trans_init.body_frame_id = "composite_body"
vehicle.trans_init.position = [6.8E6, 0.0, 0.0]

dynamics.dyn_manager.add_body_action (vehicle.trans_init)


vehicle.grav_control.source_name   = "Earth"
vehicle.grav_control.active        = True
vehicle.grav_control.spherical     = False
vehicle.grav_control.degree    = 4
vehicle.grav_control.order     = 4

vehicle.dyn_body.grav_interaction.add_control(vehicle.grav_control)

apsides_predictor.apsides.subscribe();
apsides_predictor.planet_rel_state.reference_name = "Earth"
