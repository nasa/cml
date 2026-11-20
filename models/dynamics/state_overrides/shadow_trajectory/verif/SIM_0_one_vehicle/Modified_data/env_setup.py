exec(open("../Modified_data/common_env_setup.py").read())

veh_setup( vehicle.body, "test_vehicle")
veh_setup( vehicle.shadow.body, "shadow_vehicle")

vehicle.body.rotational_dynamics = True

mass_init( vehicle.mass_init, vehicle.body)

state_init( vehicle.body)
