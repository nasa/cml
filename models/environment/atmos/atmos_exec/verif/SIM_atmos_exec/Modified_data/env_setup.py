env.de4xx.set_model_number(421)

dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_SinglePlanet
dynamics.dyn_manager_init.central_point_name = "Earth"

exec(open("Modified_data/time_default_config.py").read())
exec(open("Modified_data/set_time.py").read())
set_time_utc(2013,12,1,13,5,1.2)
#*****************************************************************************

exec(open("Modified_data/vehicle_config.py").read())
root_veh_config( veh1, "vehicle1")
sub_veh_config( veh1a, veh1, "vehicle1a")
sub_veh_config( veh1b, veh1, "vehicle1b")

root_veh_config( veh2, "vehicle2")
sub_veh_config( veh2a, veh2, "vehicle2a")
veh2.state_init.position = [0.0, 6500000.0, 500000.0]
veh2.state_init.velocity = [8000.0, 0.0, 0.0]
