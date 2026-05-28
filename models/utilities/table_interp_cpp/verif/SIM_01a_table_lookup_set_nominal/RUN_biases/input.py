exec(open("RUN_interp_all/input.py").read())
trick.add_read(0,"""
print('applying bias of 1000 to index 14 of table data')
print('Manually check testing_model.test_so_defined.single_table.data for confirmation')
testing_model.test_so_defined.single_table.bias_data(1000.0,14)
print('')
print('applying bias of +1.0 to indices 0,1,2:')
testing_model.test_so_defined.print_tiv_indep1()
testing_model.test_so_defined.indep1.bias_data( 1.0,0,2)
testing_model.test_so_defined.print_tiv_indep1()
print('')
print('applying bias of -2.0 to index 0:')
testing_model.test_so_defined.print_tiv_indep1()
testing_model.test_so_defined.indep1.bias_data(-2.0,0)
testing_model.test_so_defined.print_tiv_indep1()
testing_model.test_so_defined.indep1.bias_data(-1.0,1,2)
""")

trick.add_read(3,"""
testing_model.test_so_defined.indep2.reset_index_front()
""")

trick.add_read(5,"""
testing_model.test_so_defined.indep2.reset_index_back()
""")
trick.stop(9)
