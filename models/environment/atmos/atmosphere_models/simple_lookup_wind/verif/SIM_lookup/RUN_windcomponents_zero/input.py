exec(open( "input_common.py").read())

test_data = [ #altitude
              1.0, 2.0, 3.0, 4.0, 5.0,
              # North:
              10, 20 ,0, -0, 50,
              # East:
              20, 10, 0, -0, -20]
test.wind.assign_component_data( test_data, 2, 5)
