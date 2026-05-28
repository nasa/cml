exec(open("RUN_unit_test/input.py").read())
trick.add_read(2, """
print('testing')
print (test.lagged_altitude)
test.lagged_data.compute_with_query(  90)
test.lagged_data.compute_with_query(  91)
""")
trick.stop(3)
