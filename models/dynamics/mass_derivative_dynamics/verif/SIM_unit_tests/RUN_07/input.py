# This run tests the effect of releasing the mass from a point in the y-z plane
# relative to the propellant source while it is rotating on the z-axis.
# This should produce a torque on all 3-axes.
# Note - even though the mass-motion has no x-component, the original
# propellant position has an x-component, so the nozzzle has an x-position, so
# the angular momentum of the released mass has an x-component, so there must
# be an x-axis torque.
exec(open( "RUN_01/input.py").read())
test.R_nozzle = [1, 5, 10]
