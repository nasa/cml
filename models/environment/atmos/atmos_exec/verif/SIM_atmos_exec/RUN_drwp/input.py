exec(open("RUN_no_atmos/input.py").read())

earth.DRWP_atmos.drwpFileName = "Binaries/DRWP_no_w_comp.bin"

veh1.atmos_exec.atmos_model = trick.AtmosphereExec.ATMOS_DRWP
veh2.atmos_exec.atmos_model = trick.AtmosphereExec.ATMOS_DRWP
veh1.atmos_exec.winds_model = trick.AtmosphereExec.WINDS_DRWP
veh2.atmos_exec.winds_model = trick.AtmosphereExec.WINDS_DRWP
