exec(open("RUN_no_atmos/input.py").read())

veh1.atmos_exec.atmos_model = trick.AtmosphereExec.ATMOS_STD76
veh2.atmos_exec.atmos_model = trick.AtmosphereExec.ATMOS_STD76

veh1.atmos_exec.winds_model = trick.AtmosphereExec.WINDS_SIMPLE
veh2.atmos_exec.winds_model = trick.AtmosphereExec.WINDS_SIMPLE


test_data = [ # altitude:
              300000, 200000, 150000, 100000,
              # wind-blowing-from direction:
                 0.0,    3.0,    6.0,    1.0,
              # wind-magnitude:
                  15,     14,     13,     12]
earth.simple_wind.assign_dir_mag_data( test_data,
                                       4) #4 values per variable
