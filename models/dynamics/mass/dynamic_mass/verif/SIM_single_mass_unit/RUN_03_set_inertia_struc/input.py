# This run tests the specification flag indicating that the inertia tensor 
exec(open("RUN_02_interp/input.py").read())
mass_test.tank.dynamic_properties.interpolation.set_inertia_structural_cg_flag(True)
# do it again to test the return on setting the value to its current value:
mass_test.tank.dynamic_properties.interpolation.set_inertia_structural_cg_flag(True)
