exec(open("Modified_data/common_input.py").read())

# Call TR_param.initialize(double ref_geodetic_altitude, double ref_geodetic_latitude, double ref_longitude)
# with NULL planet pointer.  This will terminate the simulation.
vehicle.state_initialize.TR_param.initialize( 0.0, 0.0, 0.0 )

