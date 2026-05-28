# This test case initiates a 'dry mass" configuration on an empty body to
# emit an error.
exec(open("RUN_01_parallel_tanks/input.py").read())

# give the dummy_body a name
mass_test.dummy_body.name = "dummy_body"

# trigger "update mass error" in dynamic_body_mass.cc, line 376
mass_test.dummy_body.initiate_dry_mass_config();

trick.stop(1)
