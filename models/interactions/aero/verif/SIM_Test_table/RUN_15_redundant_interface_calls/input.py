exec(open("RUN_02_example_table/input.py").read())

print("\n***** Attempt to initialize the interface before the Atmos-rel-state ******")
aero.interface.initialize()
print("***** activity status = %d" %aero.interface.is_active())
print("*********************************************************")

trick.add_read(0,"""
print("***** Unsubscribe leading to deactivate -- no message ******")
aero.interface.unsubscribe()
print("***** activity status = %d" %aero.interface.is_active())
print("*********************************************************")


print("***** Update while inactive -- no message ******")
aero.interface.update()
print("***** activity status = %d" %aero.interface.is_active())
print("*********************************************************")


print("***** Subscribe while out of domain -- self-deactivates ******")
aero.geodetic_altitude = 1E10
aero.interface.subscribe()
print("***** activity status = %d" %aero.interface.is_active())
print("*********************************************************")

print("***** Disable and try to initialize -- no message ******")
aero.interface.disable()
aero.interface.initialize()
print("***** activity status = %d" %aero.interface.is_active())
print("*********************************************************")
""")
trick.stop(0)
