# This run tests the effect of releasing the mass at a point with a slight
# y-axis offset from the x-axis while it is spinning about the z-axis.
# Because the mass motion is purely in the x-y plane and the angular velocity
# is along the z-axis, the torque should be oriented along the z-axis.
# delta-Inertia ~ ((10^2) + (2^2) - (1^2) = 103.
exec(open( "RUN_01/input.py").read())
test.R_nozzle = [10,2,0]
