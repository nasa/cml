exec(open("RUN_pos_NED_Geoc/input.py").read())
vehicle.state_initialize.position_input_data_type = trick.StateInitialize.NED_Geodetic
# Purely down dir, should have equal contributions to ECEF/ECI Y-dir and Z-dir
# because of ref_point_latitude, ref_point_longitude.  more specifically
# 7071.1 in each direction
vehicle.state_initialize.position = [0.0, 0.0, 10000]
