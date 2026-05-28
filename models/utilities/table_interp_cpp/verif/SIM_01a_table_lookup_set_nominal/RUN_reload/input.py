exec(open("Log_data/log_data.py").read())

print('\n***********************************************')
print("Error message - need to first call clear_data()")
print("***********************************************")
trick.add_read(2,"""
testing_model.reload_data_indep_no_clear()
""")
trick.add_read(5,"""
testing_model.reload_data_indep()
""")
trick.add_read(7,"""
testing_model.test_so_defined.single_table_vals[1][1][1] = 5111
testing_model.reload_data_dep()
""")

trick.stop(9)
