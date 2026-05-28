exec(open("RUN_no_atmos/input.py").read())

veh1.atmos_exec.atmos_model = trick.AtmosphereExec.ATMOS_STD76
veh2.atmos_exec.atmos_model = trick.AtmosphereExec.ATMOS_STD76

veh1.atmos_exec.winds_model = trick.AtmosphereExec.WINDS_SIMPLE
veh2.atmos_exec.winds_model = trick.AtmosphereExec.WINDS_SIMPLE


altitude_component_data = [ # altitude:
                            300000, 200000, 100000,
                            # North:
                                5,      0,      3,
                            # East:
                                15,    10,     13]
earth.simple_wind.assign_component_data( altitude_component_data,
                                         2, # 2 data sets - North, East
                                         3) # 3 values per variable
