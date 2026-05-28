# This starts out the same as RUN_02.
# At t=5, it switches over to the data produced by RUN_01c, so should match
# the data from:
#   RUN_02  t < 5
#   RUN_01c t >= 5
exec(open("RUN_02_example_table/input.py").read())
drg2.add_variable("aero.interface.executive.coefficients.CX")
drg2.add_variable("aero.interface.executive.coefficients.CZ")

trick.add_read(4.0, "aero.interface.executive.change_table(aero.table_XYZ)")
trick.stop(6)
