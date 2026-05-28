exec(open( "input_common.py").read())

test_data = [ #altitude
              1.0, 2.0, 3.0, 4.0, 5.0,
              # Direction:
              -1.0, 1.0, 3.0, 5.0, 1.5,
              # Magnitude:
                11,  12,  13,  14,  15,
              # Up:
                 1,   0,  -1,  -2,  -3]

test.wind.assign_dir_mag_vert_data( test_data, 5)
