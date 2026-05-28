exec( open("RUN_R3/input.py").read())
# Align planet rotation equal to target position to elicit warning
test.planet.pfix.state.rot.ang_vel_this = [0,1,0]
trick.stop(1)
