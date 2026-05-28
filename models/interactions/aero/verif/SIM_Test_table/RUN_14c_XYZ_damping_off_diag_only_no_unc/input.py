# Just forget about uncertainty terms in off-diagonal terms
aero.table_XYZ.include_diagonal_damping_terms = True
aero.table_XYZ.include_off_diagonal_damping_terms = True
aero.table_XYZ.include_off_diagonal_damping_unc_terms = False
aero.table_XYZ.uncertainties_expressed_as_percent = True
exec(open( "RUN_01c_XYZ/input.py").read())

aero.interface.executive.dispersion_active = True
