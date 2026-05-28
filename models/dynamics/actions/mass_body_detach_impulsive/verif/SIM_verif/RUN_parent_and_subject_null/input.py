exec(open("RUN_central_impulse/input.py").read())

vehicle.bodyC.set_name("vehicleC")
mass_init (vehicle.mass_init_C, vehicle.bodyC)
vehicle.bodyD.set_name("vehicleD")
mass_init (vehicle.mass_init_D, vehicle.bodyD)
vehicle.attach_D_to_C.set_parent_body( vehicle.bodyC )
vehicle.attach_D_to_C.set_subject_body( vehicle.bodyD )
vehicle.attach_D_to_C.subject_point_name = "vehicleD"
vehicle.attach_D_to_C.parent_point_name = "vehicleC"

dynamics.dyn_manager.add_body_action( vehicle.attach_D_to_C )

vehicle.detach_D_from_C.set_subject_body( vehicle.bodyD )
vehicle.detach_D_from_C.impulse_magnitude = 5.0
vehicle.detach_D_from_C.subject_point_name = "vehicleD"
vehicle.detach_D_from_C.parent_point_name = "vehicleC"
dynamics.dyn_manager.add_body_action( vehicle.detach_D_from_C)
