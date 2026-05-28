exec(open("Log_data/log_data.py").read())
exec(open("Log_data/log_non_root_test_data.py").read())
log_test_data(1.0)
log_non_root_test_data(1.0)

exec(open("Modified_data/env_setup.py").read())

# Set up third body
vehicle.initialize_third_body = True

vehicle.bodyE.integ_frame_name = "Space.inertial"
vehicle.bodyE.set_name("vehicleE")
vehicle.bodyE.translational_dynamics = True
vehicle.bodyE.rotational_dynamics = True

mass_init (vehicle.mass_init_E, vehicle.bodyE)

vehicle.attach_E_to_B.set_parent_body( vehicle.bodyB )
vehicle.attach_E_to_B.set_subject_body( vehicle.bodyE )
vehicle.attach_E_to_B.subject_point_name = "vehicleE"
vehicle.attach_E_to_B.parent_point_name = "vehicleB"
vehicle.attach_E_to_B.active = True

dynamics.dyn_manager.add_body_action( vehicle.attach_E_to_B )

vehicle.detach_E_from_B.set_subject_body( vehicle.bodyE )
vehicle.detach_E_from_B.active = False
vehicle.detach_E_from_B.impulse_magnitude = 5.0
vehicle.detach_E_from_B.subject_point_name = "vehicleE"
vehicle.detach_E_from_B.parent_point_name = "vehicleB"
dynamics.dyn_manager.add_body_action( vehicle.detach_E_from_B)


trick.add_read(3.0, """
vehicle.detach_E_from_B.active = True
""")


trick.sim_services.exec_set_terminate_time(10)
