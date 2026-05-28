# This should return the same data as RUN_03a; it uses a different mechanism to
# switch between tables
exec(open("RUN_02_example_table/input.py").read())
drg2.add_variable("aero.interface.executive.coefficients.CX")
drg2.add_variable("aero.interface.executive.coefficients.CZ")

trick.add_read(4.0, "aero.interface.executive.change_table('XYZ')")
trick.stop(6)
