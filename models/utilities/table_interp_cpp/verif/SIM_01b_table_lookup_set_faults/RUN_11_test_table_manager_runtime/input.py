print("***********************************************************")
print("****************  TEST: BEFORE INIT ***********************")
print("***********************************************************")

print("**** query whether table is enabled: ****")
print(test.manager.is_table_interp_enabled( test.generic_table))

print("**** query whether dependent is registered with manager: ****")
print(test.is_a_depend())

exec(open("Modified_data/common_input.py").read())

print("*** TEST: Look up an independent using an unregistered name ***")
test.manager.lookup_independent("Bad Name")
print("*** TEST: Look up an independent using an empty name ***")
test.manager.lookup_independent("")

print("***********************************************************")
print("****************  TEST: AFTER INIT ***********************")
print("***********************************************************")
trick.add_read(0,"""
print("**** query whether table is enabled: ****")
print(test.manager.is_table_interp_enabled( test.generic_table))

print("**** query whether dependent is registered with manager: ****")
print(test.is_a_depend())
""")
trick.stop(0.1)
