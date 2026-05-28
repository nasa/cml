# Change the terminate_on_error flag to True in order to hit the
# fail case scenario for a null mass_pt.
exec(open("RUN_mass_pt_null/input.py").read())

vehicle.detach_B_from_A.terminate_on_error = True
