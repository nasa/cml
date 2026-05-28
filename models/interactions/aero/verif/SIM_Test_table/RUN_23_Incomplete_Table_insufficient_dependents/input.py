# Attempts to initialize a table with no specified dependents.  Fails.
exec(open( "input_common.py").read())
aero.incomplete_table.set_table_type( trick.AeroTableSetBase.AYN_unc)
aero.incomplete_table.initialize()
