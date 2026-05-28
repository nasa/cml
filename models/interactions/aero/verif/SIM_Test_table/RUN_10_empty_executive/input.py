exec(open("input_common.py").read())
trick.CMLMessage.set_publish_level (trick.CMLMessage.Inform)
print("******** Initialize empty-executive without any tables *******")
aero.empty_executive.initialize()

aero.empty_executive.subscribe()

print("******** Try to add a NULL table *******")
aero.empty_executive.add_table(None)

print("******** Try to specify usage of a nonexistent table *******")
aero.empty_executive.change_table(2)


print("******** Try to specify usage of a nonexistent table *******")
aero.empty_executive.change_table("junk")

# Add a real table to the executive and try to initialize again
print("******** Add a real table and initialize *******")
aero.empty_executive.add_table(aero.example_table)
#aero.empty_executive.load_all_tables_at_init = True
aero.empty_executive.initialize()

print("******** Try to specify usage of a nonexistent table *******")
aero.empty_executive.change_table(2)

print("******** Try to specify usage of a nonexistent table *******")
aero.empty_executive.change_table("junk")

print("******** Specify changeover to current table by index (no effect) *******")
aero.empty_executive.change_table(0)

print("******** Specify changeover to current table by name (no effect) *******")
aero.empty_executive.change_table("example_table")

print("******** Specify changeover to current table by reference (no effect) *******")
aero.empty_executive.change_table(aero.example_table)

# Check no action on redundant initailization call:
print("******** Send initialize instruction again *******")
aero.empty_executive.initialize()

print("******** Add table again *******")
aero.empty_executive.add_table(aero.example_table)

print("******** Add empty table with same name (2 message, there are 2 tables with this name) *******")
aero.empty_executive.add_table(aero.empty_table)



print("\n********* END OF TESTS **********\n")
aero.interface.disable()
trick.stop(0)

