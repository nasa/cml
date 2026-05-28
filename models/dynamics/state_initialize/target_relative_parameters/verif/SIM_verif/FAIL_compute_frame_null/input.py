exec( open("Modified_data/env_setup.py").read())
test.framework_provides_R3 = False
test.framework.enabled = False
test.params_in.Lambda = 1.1
test.params_in.theta_Rng = 1.0
test.params_in.Vmag = 1
test.initialize_with_null_R3 = True
trick.stop(0)
