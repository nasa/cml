exec(open("Log_data/log_data.py").read())

test.lagged_atmosphere.max_delta_altitude = 30
test.lagged_winds.max_delta_altitude = 30

# Insert specific initial conditions in the Unit_test files:
test.framework.data_file_name = "Unit_test_data/data.txt"
test.framework.vars_file_name = "Unit_test_data/variables.txt"

# Set the termination time.
trick.stop(10)


trick.add_read(0,"""
print('************* Test 1 ****************************')
print('Add the first nodes at {100,90}')
print('Target altitude is 100')
print('Interpolation Success, pulls the 100 node')
print('*************************************************')
""")

trick.add_read(1,"""
print('')
print('******************* Test 2 ****************************')
print('Add another same node at 90, call this 90a')
print('Nodes at {100, 90, 90a}')
print('Target altitude is 91')
print('Interpolation Success, interpolates between {100,90}')
print('*******************************************************')
""")

trick.add_read(2,"""
print('******************* Test 3 ****************************')
print('Add another node at 80; nodes are at {100, 90, 90a, 80}')
print('Target altitude is 85')
print('Interpolation Success, interpolates between {90a,80}')
print('*******************************************************')
""")

trick.add_read(3,"""
print('******************* Test 4 ****************************')
print('Add another node at 40')
print('This strips the nodes at {100, 90}')
print('Nodes are now at {90a, 80, 40}')
print('Target altitude is 50')
print('Interpolation Success, interpolates between {80,40}')
print('*******************************************************')
""")

trick.add_read(4,"""
print('******************* Test 5 ****************************')
print('Add another node at 80 (80a); nodes are at {90a, 80, 40, 80a}')
print('Target altitude is 90')
print('Interpolation Success, pulls the 90a node')
print('*******************************************************')
""")

trick.add_read(5,"""
print('******************* Test 6 ****************************')
print('Add another node at 40 (40a); nodes are at {90, 80, 40, 80a, 40a}')
print('Target altitude is 50')
print('Interpolation Success, interpolates between {80,40}')
print('*******************************************************')
""")

trick.add_read(6,"""
print('******************* Test 7 ****************************')
print('Add another node at 40 (40b)')
print('This strips the node at 90; ; nodes are at {80, 40, 80a, 40a, 40b}.')
print('Target altitude is 40')
print('Interpolation Success, pulls the 40 node')
print('*******************************************************')
""")

trick.add_read(7,"""
print('******************* Test 8 ****************************')
print('Add another node at 70; nodes are at {80, 40, 80a, 40a, 40b, 70}')
print('Target altitude is 70')
print('Interpolation Success, interpolates between {80,40}')
print('and does not pull data from the new 70 node.')
print('*******************************************************')
""")

trick.add_read(8,"""
print('******************* Test 9 ****************************')
print('Add a node at 30; nodes are at {80, 40, 80a, 40a, 40b, 70, 30}')
print('Target altitude is 35')
print('Interpolation Success, interpolates between {70,30}')
print('*******************************************************')
""")

trick.add_read(9,"""
print('******************* Test 10 ****************************')
print('Add a node at 10, stripping the node at 80')
print('Nodes are at {40, 80a, 40a, 40b, 70, 30, 10}')
print('Target altitude is 75')
print('Interpolation Success, interpolates between {80a,40a}')
print('*******************************************************')
""")
