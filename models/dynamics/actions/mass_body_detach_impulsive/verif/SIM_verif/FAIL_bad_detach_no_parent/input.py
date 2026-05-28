exec(open("RUN_central_impulse/input.py").read())

#Changing the subject to the root body so that it has no parent to detach from.
vehicle.detach_B_from_A.set_subject_body(vehicle.bodyA)
