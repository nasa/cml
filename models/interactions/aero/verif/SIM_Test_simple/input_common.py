aero.interface.subscribe()

# Put some dummy values in for environment factors; the environment is not
# being computed and without these we would get div-0 warnings.
aero.atmos_exec.out.wind_velocity_eci = [1.0, 0.0, 0.0]
aero.atmos_exec.out.speed_of_sound = 1.0;
aero.geodetic_altitude = 100;

aero.framework.data_file_name = "Unit_test_data/data.txt"
aero.framework.vars_file_name = "Unit_test_data/variables.txt"

trick.stop(6)
