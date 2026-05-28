exec(open("RUN_no_atmos/input.py").read())

# Ask to use an WindsOption that doesn't exist
veh1.atmos_exec.winds_model = -1
veh2.atmos_exec.winds_model = -1
