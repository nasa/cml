exec( open("RUN_R3/input.py").read())
# Set planet rotation to zero to elicit warnings
test.planet.pfix.state.rot.ang_vel_this = [0,0,0]
trick.stop(1)
