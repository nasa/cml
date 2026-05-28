# This run tests the effect of releasing the mass along the x-axis while it is
# spinning about the  y- and z-axes.  Because the mass motion is purely x-axis,
# it should have the same delta-inertia effect on the y- and z-axes and the
# torque on both axes should match and have the same component on each axis as
# RUN_01 delta-Inertia ~ (10^2) - (1^2) = 99.
exec(open( "RUN_01/input.py").read())
test.body.composite_body.state.rot.ang_vel_this = [0,1,1]
