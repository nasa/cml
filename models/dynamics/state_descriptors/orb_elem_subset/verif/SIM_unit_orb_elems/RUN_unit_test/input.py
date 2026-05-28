exec(open( "Log_data/log_data.py").read())
log_data( 1.0 )

dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.central_point_name = "Earth"
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_SinglePlanet

jeod_time.time_manager_init.initializer = "UTC"
jeod_time.time_manager_init.sim_start_format = trick.TimeEnum.calendar
jeod_time.time_tai.update_from_name = "Dyn"
jeod_time.time_tai.initialize_from_name = "UTC"
jeod_time.time_tt.update_from_name = "TAI"
jeod_time.time_tt.initialize_from_name = "TAI"
jeod_time.time_ut1.update_from_name = "TAI"
jeod_time.time_ut1.initialize_from_name = "TAI"
jeod_time.time_gmst.update_from_name = "UT1"
jeod_time.time_gmst.initialize_from_name = "UT1"
jeod_time.time_utc.update_from_name = "TAI"
jeod_time.time_utc.calendar_year = 2016
jeod_time.time_utc.calendar_month = 6
jeod_time.time_utc.calendar_day = 22
jeod_time.time_utc.calendar_hour = 16
jeod_time.time_utc.calendar_minute = 0
jeod_time.time_utc.calendar_second = 0.0

oe_verif.body.integ_frame_name = "Earth.inertial"
oe_verif.body.set_name("composite_body")
oe_verif.body.translational_dynamics = True
oe_verif.body.rotational_dynamics = False

oe_verif.framework.data_file_name = "Unit_test/data.txt"
oe_verif.framework.vars_file_name = "Unit_test/variables.txt"

trick.stop(7.0)
