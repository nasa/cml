# This run tests the effect of releasing the mass at a point with a slight
# y-axis offset from the x-axis while it is spinning about the y- and z-axes.
# Because the mass motion is in the x-y plane, there is an x-y cross-coupling
# in the delta-inertia that should produce a torque about the x-axis of
# magnitude (xy)_N - (xy)_P = 20 - 0 = 20.
# There is no angular velocity on the x-axis, so the cross-coupling does not
# affect the torque on the y-axis, which should therefore match the "99" from
# RUN_02.
# The torque on the z-axis should match the "103" from RUN_03 because there is
# no cross-coupling associated with the z-axis.
exec(open( "RUN_01/input.py").read())
test.R_nozzle = [10,2,0]
test.body.composite_body.state.rot.ang_vel_this = [0,1,1]
