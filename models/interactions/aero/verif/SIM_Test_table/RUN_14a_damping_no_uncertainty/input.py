# include the damping terms, but forget to include the dispersion of those terms
# Leads to warning messages.
aero.table_XYZ.include_diagonal_damping_terms = True
aero.table_XYZ.include_off_diagonal_damping_terms = True
aero.table_XYZ.include_diagonal_damping_unc_terms = False
aero.table_XYZ.include_off_diagonal_damping_unc_terms = False
exec(open( "RUN_01c_XYZ/input.py").read())
