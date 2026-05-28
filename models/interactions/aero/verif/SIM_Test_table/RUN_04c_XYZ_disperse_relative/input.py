# The RUN_04* run-set tests the computation and application of biases for each
# of the table-types, with uncertainties expressed relative to the coefficients
# so that they get recomputed every cycle.
aero.table_XYZ.uncertainties_expressed_as_percent = True
exec(open( "RUN_01c_XYZ/input.py").read())

aero.interface.executive.dispersion_active = True
