# The RUN_05* run-set tests the computation and application of biases for each
# of the table-types, with uncertainties expressed as absolute values.  For
# AYN_unc, the biases will still be computed every cycle because the
# uncertainties themselves will be variable.
exec(open( "RUN_01e_AYN_unc/input.py").read())

aero.interface.executive.dispersion_active = True
