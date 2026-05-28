# specify the dispersions as relative to the coefficient value.
# This will force the re-computation/application of the biases on every cycle.
# Compare RUN_06c, RUN_06d, RUN_06e
# coefficient values from RUN_06d should be some constant (per coefficient) value larger than
# those from RUN_06c.  Coefficient values from RUN_06e should be larger by the fraction
# e.g. if values in RUN_06c are {1.1, 2.2, 3.3}
#      and values in RUN_06d are 0.1 higher: {1.2, 2.3, 3.4}
#      then these values should be 0.1x (or 10%) higher: {1.21, 2.42, 3.63}
aero.table_XYZ.uncertainties_expressed_as_percent = True
exec(open( "RUN_06d_damping_dispersion_absolute/input.py").read())
