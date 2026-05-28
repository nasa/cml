# This run tests the effect of releasing the mass along the z-axis while it is
# spinning about the z-axis.  Because the propellant is still coming from the
# x-axis, the net effect should be due to internal motion only, producing zero
# force and a small positive torque about the z-axis.
exec(open( "RUN_01/input.py").read())
test.R_nozzle = [0, 0, 10]
