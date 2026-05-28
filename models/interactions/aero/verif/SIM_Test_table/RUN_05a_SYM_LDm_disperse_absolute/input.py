# The RUN_05* run-set tests the computation and application of biases for each
# of the table-types, with uncertainties expressed as absolute values so they
# only require computing once.
exec(open( "RUN_01a_SYM_LDm/input.py").read())

aero.interface.executive.dispersion_active = True
