# This run tests the effect of releasing the mass along the x-axis while it is
# spinning about the x-axis, with the propellant also located on the x-axis.
# The effect should be NULL.
exec(open( "RUN_01/input.py").read())
test.body.composite_body.state.rot.ang_vel_this = [1,0,0]
