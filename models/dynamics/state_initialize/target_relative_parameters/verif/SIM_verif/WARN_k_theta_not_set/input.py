exec( open("RUN_R3/input.py").read())
# Set input velocity and planet rotation to zero to elicit warnings/errors
test.K_theta = 0.0
trick.stop(1)
