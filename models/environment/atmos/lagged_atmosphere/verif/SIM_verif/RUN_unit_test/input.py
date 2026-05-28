exec(open("Log_data/log_data.py").read())

test.lagged_data.set_max_delta_altitude(30)

# Insert specific initial conditions in the Unit_test files:
test.framework.data_file_name = "Unit_test_data/data.txt"
test.framework.vars_file_name = "Unit_test_data/variables.txt"

# Set the termination time.
trick.stop(10)


trick.add_read(0,"""
print('************* Test 1 ****************************')
print('Add the first node at 100')
print('Interpolation Error - table has only a single set')
print('*************************************************')
""")

trick.add_read(1,"""
print('')
print('******************* Test 2 ****************************')
print('Add the same node at 100 (skipped)')
print('Interpolation Error - table still has only a single set')
print('*******************************************************')
""")

trick.add_read(2,"""
print('')
print('******************* Test 3 ****************************')
print('Add a node at 90; nodes are at {100, 90}')
print('Target altitude is 100')
print('Interpolation Error - input above table')
print('*******************************************************')
""")

trick.add_read(3,"""
print('******************* Test 4 ****************************')
print('Add another node at 90 (skipped); nodes are at {100, 90}')
print('Target altitude is 90')
print('Interpolation Success, pulls the 90 node')
print('*******************************************************')
""")

trick.add_read(4,"""
print('******************* Test 5 ****************************')
print('Add another node at 80; nodes are at {100, 90, 80}')
print('Target altitude is 85')
print('Interpolation Success, interpolates between {90,80}')
print('*******************************************************')
""")

trick.add_read(5,"""
print('******************* Test 6 ****************************')
print('Add another node at 40')
print('This strips the node at 100.')
print('Nodes are now at {90, 80, 40}')
print('Target altitude is 50')
print('Interpolation Success, interpolates between {80,40}')
print('*******************************************************')
""")

trick.add_read(6,"""
print('******************* Test 7 ****************************')
print('Add another node at 80 (skipped); nodes are at {90, 80, 40}')
print('Target altitude is 90')
print('Interpolation Error - input above table')
print('*******************************************************')
""")

trick.add_read(7,"""
print('******************* Test 8 ****************************')
print('Add another node at 40 (skipped); nodes are at {90, 80, 40}')
print('Target altitude is 50')
print('Interpolation Success, interpolates between {80,40}')
print('using the original data for the {80,40} nodes')
print('*******************************************************')
""")

trick.add_read(8,"""
print('******************* Test 9 ****************************')
print('Add another node at 40 (skipped).')
print('This strips the node at 90; ; nodes are at {80, 40}.')
print('Target altitude is 30')
print('Interpolation Error - input below table')
print('*******************************************************')
""")

trick.add_read(9,"""
print('******************* Test 10 ****************************')
print('Add another node at 70 (skipped); nodes are at {80, 40}')
print('Target altitude is 70')
print('Interpolation Success, interpolates between {80,40}')
print('and does not pull data from the failed 70 node.')
print('*******************************************************')
""")

trick.add_read(10,"""
print('******************* Test 11 ****************************')
print('Set min-delta-altitude to 15')
print('Add another node at 30 (skipped); nodes are at {80, 40}')
print('Target altitude is 35')
print('Interpolation Error - input below table')
print('*******************************************************')
test.lagged_data.set_min_delta_altitude(15)
""")
