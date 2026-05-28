exec(open("RUN_no_atmos/input.py").read())

# Call the GramInterface initialize() method which can only happen
# if that base class is not derived from properly
trick.add_read(0.0, """
earth.gram_i.initialize(0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0.0)
""")

