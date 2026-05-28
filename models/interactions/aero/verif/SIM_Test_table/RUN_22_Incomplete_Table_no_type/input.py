exec(open( "input_common.py").read())

# Either of these do the same job -- they both result in the failed
# initialization because of an unspecified data type.  The first additionally
# tests the load-all-tables-at-init capability, so go with that as default.
aero.interface.executive.load_all_tables_at_init = True

# Optional alternative:
#aero.incomplete_table.initialize()
