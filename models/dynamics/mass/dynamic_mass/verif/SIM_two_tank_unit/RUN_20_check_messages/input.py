exec(open("RUN_01_parallel_tanks/input.py").read())
# NOTE - messages at dyn_mass_string.cc:131 picked up by unit-test

#pick up error at dynamic_mass_group.cc:68
mass_test.group.add_string_to_group( mass_test.string)

# pick up error at dynamic_mass_group.cc:354
mass_test.group.add_mass_to_group( mass_test.tank_a)

# pick up error at dynamic_mass_string.cc:237
mass_test.string.add_mass_to_string( mass_test.tank_a)
