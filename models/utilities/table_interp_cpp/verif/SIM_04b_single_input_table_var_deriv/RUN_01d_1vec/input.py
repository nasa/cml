exec(open("Log_data/log_data.py").read())
test.tab_set.subscribe()

test.interpolation_table_1vec.set_output(test.x_vec);
test.interpolation_table_1vec.derivs.set_output(test.xdot_vec);
test.tab_set.add_table(test.interpolation_table_1vec);
test.tab_set.associate_table_and_independent();

trick.stop(21)
