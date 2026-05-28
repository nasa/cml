exec(open("Modified_data/common_input.py").read())

vehicle.state_initialize.position_input_data_type = trick.StateInitialize.NED_Geocentric
vehicle.state_initialize.ref_point_altitude =  100000
vehicle.state_initialize.ref_point_latitude =  math.pi/4 # y and z the same
vehicle.state_initialize.ref_point_longitude = math.pi/2 # x = 0
vehicle.state_initialize.position = [0.0, 0.0, 0.0]
