vehicle.detach_B_from_A.terminate_on_error = False

# Attempt the bad detachment first, then run env_setup.py to set up a valid JEOD
# sim state.
exec(open("FAIL_bad_detach_no_subject/input.py").read())

exec(open("Modified_data/env_setup.py").read())
