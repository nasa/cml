exec( open("Modified_data/env_setup.py").read())
test.framework_provides_R3 = False
test.framework.enabled = False
test.params_in.Lambda = 1.1
test.params_in.theta_Rng = 1.0
test.params_in.Vmag = 1
test.tr_params_to_R3.set_planet(None)
trick.stop(0)
