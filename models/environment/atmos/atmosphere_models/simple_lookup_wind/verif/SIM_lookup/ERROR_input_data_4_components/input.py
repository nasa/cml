exec(open( "input_common.py").read())

test_data = [ #altitude
               1,   2,   3,   4,   5,
              # North:
              10,  20,  30,  40,  50,
              # East:
              20,  10,   0, -10, -20,
              # Down:
              -1,   0,   1,   2,   3]

test.wind.assign_component_data( test_data, 4, 5)
