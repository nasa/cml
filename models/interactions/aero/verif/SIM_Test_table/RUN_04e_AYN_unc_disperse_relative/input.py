# The RUN_04* run-set tests the computation and application of biases for each
# of the table-types, with uncertainties expressed relative to the coefficients
# so that they get recomputed every cycle.
aero.table_AYN_unc.uncertainties_expressed_as_percent = True
exec(open( "RUN_01e_AYN_unc/input.py").read())

aero.interface.executive.dispersion_active = True
