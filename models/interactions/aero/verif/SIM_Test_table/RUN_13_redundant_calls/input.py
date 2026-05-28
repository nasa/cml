# should result in the executive returning immediately.
exec(open("RUN_02_example_table/input.py").read())
trick.CMLMessage.set_publish_level (trick.CMLMessage.Inform)
trick.add_read(0.0, """
print('************ Redundant request to change to "example_table" **********')
aero.interface.executive.change_table('example_table')

print('************ Request to re-initialize "example_table" **********')
print('NO MESSAGE')
aero.example_table.initialize()

print('************ Request to change type of "example-table" **********')
aero.example_table.set_table_type( trick.AeroTableSetBase.AYN)

print('************ Request to initialize "empty_table" **********')
aero.empty_table.initialize()
""")
trick.stop(0)
