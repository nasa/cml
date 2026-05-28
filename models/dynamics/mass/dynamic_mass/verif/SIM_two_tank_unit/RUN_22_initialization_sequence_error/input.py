# This test is initializing an empty body to emit an error.
exec(open("RUN_01_parallel_tanks/input.py").read())

# give the dummy_body a name
mass_test.dummy_body.name = "dummy_body"

# trigger "Initialization sequence error" in dynamic_body_mass.cc, line 79
mass_test.dummy_body.initialize_dyn_mass()

trick.stop(1)
