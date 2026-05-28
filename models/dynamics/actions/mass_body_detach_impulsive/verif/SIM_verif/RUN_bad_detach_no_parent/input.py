# Tests the detach of a root body with terminate_on_error turned off.
exec(open("FAIL_bad_detach_no_parent/input.py").read())

vehicle.detach_B_from_A.terminate_on_error = False
