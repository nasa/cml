exec(open("Log_data/log_data.py").read())
test.tab_set.subscribe()

test.tab_set.add_table(test.interpolation_table_1elem);
test.tab_set.associate_table_and_independent();

trick.stop(21)
