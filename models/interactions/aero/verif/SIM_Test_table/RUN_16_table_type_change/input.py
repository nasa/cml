# Attempt to change table type after initialization should fail.
exec(open( "RUN_02_example_table/input.py").read())

trick.add_read(0.0, """
print('**** Cannot change table type message: *****')
aero.example_table.set_table_type (2)
""")
