exec(open("RUN_no_atmos/input.py").read())

# Ask to use an AtmosphereOption that doesn't exist
veh1.atmos_exec.atmos_model = -1
veh2.atmos_exec.atmos_model = -1
