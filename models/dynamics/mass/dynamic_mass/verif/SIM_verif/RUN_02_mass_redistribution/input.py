# This run tests the effect of the delta_mass_is_redistributed flag.
# With this flag, the composite-body state should not be affected
# by mass changes because it is assumed that all mass changes are redistributions,
# i.e. the mass remains in the vehicle.
exec(open("RUN_01_nominal/input.py").read())
vehicle.group.delta_mass_is_redistributed = True
