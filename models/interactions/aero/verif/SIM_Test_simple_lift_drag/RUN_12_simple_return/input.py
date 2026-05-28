exec(open("input_common.py").read())

print("****** Testing unneeded initialize call ************")
print("**************** No messages ***********************\n\n")
trick.add_read(
    0.0,
    """
aero.interface.initialize()
aero.interface.disable()
aero.interface.initialize()
""",
)

trick.stop(0)
