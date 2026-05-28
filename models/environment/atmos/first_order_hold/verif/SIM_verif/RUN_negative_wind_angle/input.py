exec(open("RUN_hold/input.py").read())

test_object.atmos_out.wind_velocity_td[0] = 80.0
test_object.atmos_out.wind_velocity_td[1] = -20.0
test_object.atmos_out.wind_velocity_td[2] = 800.0

test_object.state.topodetic.T_inrtl_to_this = [[0,0,1],[0,1,0],[-1,0,0]]
test_object.state.topocentric.T_inrtl_to_this = [[1,0,0],[0,1,0],[0,0,1]]
