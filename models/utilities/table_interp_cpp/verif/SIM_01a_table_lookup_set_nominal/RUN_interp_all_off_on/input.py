# This run tests the ability to turn a table on and off mid-sim.  There are 2
# managers and two tables per manager.  Only the multi-table in the
# test_so_defined manager is affected; this sets the values for
# test_so_defined.dependent_vars[0-1].
# test_on_the_fly.dependent_vars[0-2] and test_so_defined.dependent_vars[2]
# should be unaffected.
exec(open("RUN_interp_all/input.py").read())

trick.add_read(20, """
testing_model.test_so_defined.table_set.enable_table_interp(
                              testing_model.test_so_defined.multi_table, False)
""")
trick.add_read(50, """
testing_model.test_so_defined.table_set.enable_table_interp(
                              testing_model.test_so_defined.multi_table, True)
""")
