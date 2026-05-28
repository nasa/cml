log_extended = 0 # switch to 1 for more comprehensive logging.
exec(open("Log_data/log_data.py").read())
exec(open("Modified_data/env_setup.py").read())

vehicle.month_override = 6

jeod_time.time_manager_init.initializer = "UT1"
jeod_time.time_manager_init.sim_start_format = trick.TimeEnum.calendar
jeod_time.time_tai.update_from_name = "Dyn"
jeod_time.time_tai.initialize_from_name = "UT1"
jeod_time.time_ut1.update_from_name = "TAI"
jeod_time.time_utc.update_from_name = "TAI"
jeod_time.time_utc.initialize_from_name = "TAI"

jeod_time.time_ut1.calendar_year = 2012
jeod_time.time_ut1.calendar_month = 2
jeod_time.time_ut1.calendar_day = 4
jeod_time.time_ut1.calendar_hour = 8
jeod_time.time_ut1.calendar_minute = 28
jeod_time.time_ut1.calendar_second = 30

vehicle.body_state.reference_name = "Earth"

vehicle.atmos_exec.atmos_model = trick.AtmosphereExec.ATMOS_STD76
vehicle.dchute.atmos_exec.atmos_model = trick.AtmosphereExec.ATMOS_STD76
vehicle.mchute.atmos_exec.atmos_model = trick.AtmosphereExec.ATMOS_STD76
vehicle.schute.atmos_exec.atmos_model = trick.AtmosphereExec.ATMOS_STD76

vehicle.atmos_exec.winds_model = trick.AtmosphereExec.WINDS_SIMPLE
vehicle.dchute.atmos_exec.winds_model = trick.AtmosphereExec.WINDS_SIMPLE
vehicle.mchute.atmos_exec.winds_model = trick.AtmosphereExec.WINDS_SIMPLE
vehicle.schute.atmos_exec.winds_model = trick.AtmosphereExec.WINDS_SIMPLE

# Taken from RUN_simple_winds_components/input.py example
altitude_component_data = [ # altitude:
                            300000, 200000, 100000,
                            # North:
                                5,      0,      3,
                            # East:
                                15,    10,     13]
vehicle.simple_wind.assign_component_data( altitude_component_data,
                                         2, # 2 data sets - North, East
                                         3) # 3 values per variable

vehicle.body_state.subscribe()
vehicle.body_state.subscribe_topocentric_altitude()
vehicle.body_state.subscribe_topocentric()
vehicle.body_state.subscribe_topodetic()

vehicle.atmos_exec.subscribe();

trick.add_read(10.0, """
vehicle.dchute.subscribe()""")
trick.add_read(15.0, """
vehicle.dchute_offset = 100""")

trick.add_read(20.0, """
vehicle.mchute.subscribe()""")
trick.add_read(23.0, """
vehicle.mchute_offset = 100""")
trick.add_read(26.0, """
vehicle.mchute_offset = 200""")

trick.add_read(30.0, """
vehicle.state_override_flag = False""")

trick.add_read(40.0, """
vehicle.schute.subscribe()""")
trick.add_read(43.0, """
vehicle.schute_offset = 100""")
trick.add_read(46.0, """
vehicle.schute_offset = 200""")

trick.add_read(50.0, """
vehicle.dchute.unsubscribe()""")

trick.stop(55.0)
