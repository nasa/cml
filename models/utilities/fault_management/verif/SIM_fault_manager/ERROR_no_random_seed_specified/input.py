# Purpose: To test the defaulting of a seed to a random value if it is assigned
# to 0.
exec(open("RUN_random_walk/input.py").read())
test_object.faults.set_fault_param("Gaussian Random-Walk Fault", "seed", 0)
trick.stop(1)
