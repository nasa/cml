exec(open("Modified_data/common_input.py").read())

# Reference position vector:  Equator at the prime meridian
# Target position vector:     At North pole
# Rotation angle:             -90 deg --> Range angle will be along equator
# Range angle:                 90 deg --> Position vector will be rotated 90 deg along the equator from the target
# Position should end up on equator at 90 deg longitude
#
# From an off-line Matlab script, the position should be:
#   position = [+3.980136999662673e-10, +6.500056999999999e+06, +3.980136999662673e-10]

# Set the planet pointer in TR_param
vehicle.state_initialize.TR_param.set_planet(trick.get_address("earth.planet"));

# Initialize TR_param with a reference position at the North pole
vehicle.state_initialize.TR_param.initialize(0, math.pi/2, 0);

# Compute parameters with co-aligned position and velocity vectors.
# This will result in a warning.
vehicle.state_initialize.TR_param.compute_params_from_vectors_SWIG( [1,0,0],
                                                                    [1,0,0],
                                                                    vehicle.param_set);
