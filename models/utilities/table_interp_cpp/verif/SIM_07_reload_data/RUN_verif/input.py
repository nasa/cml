exec(open("Log_data/log_data.py").read())


independent_data_1 = [ 0.0,  1.0,  2.0,  3.0,  4.0,  5.0,  6.0]
independent_data_2 = [10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0]
independent_data_3 = [20.0, 21.0, 22.0, 23.0, 24.0]

num_indep_1 = 7
num_indep_2 = 7
num_indep_3 = 5

dependent_data_1 = [ 10.0,  11.0,  12.0,  13.0,  14.0,  15.0,  16.0,
                    110.0, 111.0, 112.0, 113.0, 114.0, 115.0, 116.0]
dependent_data_2 = [210.0, 211.0, 212.0, 213.0, 214.0, 215.0, 216.0,
                   -210.0,-211.0,-212.0,-213.0,-214.0,-215.0,-216.0,
                      5.0,   4.0,   3.0,   2.0,   1.0,   0.0,  -1.0]
dependent_data_3 = [310.0, 311.0, 312.0, 313.0, 314.0,
                   -310.0,-311.0,-312.0,-313.0,-314.0,
                      5.0,   4.0,   3.0,   2.0,   1.0,
                     -2.0,  -1.0,   0.0,   1.0,   2.0]

num_dep_1 = 2
num_dep_2 = 3
num_dep_3 = 4


# To start with, load up independent_data_1 and dependent_data_1
multi.load_independent_variable( independent_data_1,
                                 num_indep_1)

multi.load_dependent_variables( dependent_data_1,
                                num_dep_1,
                                num_indep_1)


# Set independent-variable to 1.7 (first update will have incremented value 2.7)
# Reload the dependent data to use dependent_data_2
trick.add_read(3.0, """
print('\\n' \
'*********************************************************************\\n' \
't=3 Reload dependent data\\n' \
'*********************************************************************\\n')
multi.independent_variable = 1.7
multi.load_dependent_variables( dependent_data_2,
                                num_dep_2,
                                num_indep_1)
""")

# Set independent-variable to 9.4 (first update will have value 10.4)
# Reload the independent data
# Reload the independent and dependent data
trick.add_read(6.0, """
print('\\n' \
'*********************************************************************\\n' \
't=6 Reload independent data\\n' \
'*********************************************************************\\n')
multi.independent_variable = 9.4
multi.load_independent_variable( independent_data_2,
                                 num_indep_2)
""")

# Set independent-variable to 19.4
# Reload the independent and dependent data, loading dependent first
trick.add_read(9.0, """
print('\\n' \
'*********************************************************************\\n' \
't=9 Reload dependent then independent data\\n' \
'*********************************************************************\\n')
multi.independent_variable = 19.4
multi.table_set.comprehensive_data_reload = True
multi.load_dependent_variables( dependent_data_3,
                                num_dep_3,
                                num_indep_3)
multi.load_independent_variable( independent_data_3,
                                 num_indep_3)
""")


# Repeat t=6->8
# Set independent-variable to 9.4
# Reload the independent and dependent data, loading independent first
trick.add_read(12.0, """
print('\\n' \
'*********************************************************************\\n' \
't=12 Reload independent then dependent data\\n' \
'*********************************************************************\\n')
multi.independent_variable = 9.4
multi.table_set.comprehensive_data_reload = True
multi.load_independent_variable( independent_data_2,
                                 num_indep_2)
multi.load_dependent_variables( dependent_data_2,
                                num_dep_2,
                                num_indep_2)
""")

# Reload the independent, violating consistency checks
trick.add_read(18.0, """
print('\\n' \
'*********************************************************************\\n' \
't=18 Reload independent data, inconsistent with dependent data\\n' \
'*********************************************************************\\n')
multi.independent_variable = 19.4
multi.load_independent_variable( independent_data_3,
                                 num_indep_3)
""")

# Revert the independent
trick.add_read(21.0, """
print('\\n' \
'*********************************************************************\\n' \
't=21 Revert independent data, and re-initialize\\n' \
'*********************************************************************\\n')
multi.independent_variable = 9.4
multi.load_independent_variable( independent_data_2,
                                 num_indep_2)
multi.table_set.initialize()
""")

# Reload the dependent, violating consistency checks
trick.add_read(24.0, """
print('\\n' \
'*********************************************************************\\n' \
't=24 Reload dependent data, inconsistent with independent data\\n' \
'*********************************************************************\\n')
multi.independent_variable = 9.4
multi.load_dependent_variables( dependent_data_3,
                                num_dep_3,
                                num_indep_3)
""")

trick.stop(26)
