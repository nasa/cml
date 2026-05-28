exec(open("Log_data/log_data.py").read())

exec(open("Modified_data/env_setup.py").read())

# Initial State of vehicle
# Set up a near-circular equatorial orbit starting over latitude=0, longitude=0
vehicle.state_initialize.set_subject_body(vehicle.body)
vehicle.state_initialize.body_frame_id = "composite_body"
vehicle.state_initialize.planet_name = "Earth"

vehicle.state_initialize.position_input_data_type = trick.StateInitialize.NED_Geocentric
vehicle.state_initialize.velocity_input_data_type = trick.StateInitialize.NED_Geocentric
vehicle.state_initialize.attitude_input_data_type = trick.StateInitialize.MatrixInertialBody
vehicle.state_initialize.att_rate_input_data_type = trick.StateInitialize.BodyInertialRate
vehicle.state_initialize.ref_point_altitude = 330000
vehicle.state_initialize.ref_point_latitude = 0.0
vehicle.state_initialize.ref_point_longitude = 0.0
vehicle.state_initialize.position = [0, 0, 0]
vehicle.state_initialize.velocity = [0, 7219.30029941412, 0]
vehicle.state_initialize.T_inrtl_body = [[1,0,0],[0,1,0],[0,0,1]]

vehicle.state_initialize.roll_rate_body = 0.0
vehicle.state_initialize.pitch_rate_body = 0.0
vehicle.state_initialize.yaw_rate_body = 0.0

vehicle.state_initialize.active = True
dynamics.dyn_manager.add_body_action( vehicle.state_initialize)

exec(open("Modified_data/time.py").read())


########################################################################
# Set up a RangeToPfixReference which will describe the position (Range) of
# a reference point relative to the vehicle pfix position
########################################################################

vehicle.range_to_pfix.subscribe();

vehicle.range_to_pfix.reference_data.position_type = trick.PfixReferencePoint.topocentric
vehicle.range_to_pfix.reference_data.latitude  = 0.0
vehicle.range_to_pfix.reference_data.longitude = 0.0
vehicle.range_to_pfix.reference_data.altitude  = 0.0

vehicle.range_to_pfix.reference_data.direction_type = trick.PfixReferencePoint.pfix

########################################################################
# Set up a RangeFromPfixReference which will describe the position (Range) of
# a vehicle pfix position relative to a reference point / direction
########################################################################

vehicle.range_from_pfix.subscribe();

vehicle.range_from_pfix.reference_data.position_type = trick.PfixReferencePoint.topocentric
vehicle.range_from_pfix.reference_data.latitude  = 0.0
vehicle.range_from_pfix.reference_data.longitude = 0.0
vehicle.range_from_pfix.reference_data.altitude  = 0.0

vehicle.range_from_pfix.reference_data.direction_type = trick.PfixReferencePoint.topocentric
# Set the direction to be pointing in the same direction the vehicle will be traveling, due east
vehicle.range_from_pfix.reference_data.azimuth   = 90.0 / 180.0 * 3.14159265358979

trick.stop(5000.0)

