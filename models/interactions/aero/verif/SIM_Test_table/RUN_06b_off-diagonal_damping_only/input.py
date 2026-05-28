# Include only the off-diagonal damping terms (and their uncertainties)
# Without dispersions active, this should do nothing,  match RUN_01c.
aero.table_XYZ.include_off_diagonal_damping_terms = True
exec(open( "RUN_06_baseline_no_damping/input.py").read())
