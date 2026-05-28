#!/usr/bin/env python3
# This script provides independent verification of the unit tests
# for the range model.  It reads the same inputs as the unit sim
# and it's outputs can be compared directly to those of the unit sim
# runs.  Additionally this script can be imported to provide
# generically useful functionality in verifying other models, such as
#    def geo_to_ecef (lat, lon, alt):
#    def ellip_to_sphere (lat, lon, alt):
#    def sphere_to_ellip (lat, lon, alt):
#    def ellip_to_cart (lat, lon, alt):
#    def spher_to_cart(lat, long, alt):
#    def cart_to_sphere (x, y, z):
#    def cart_to_ellip(x, y, z):

import pdb
import sys, os, inspect
import numpy as np
from argparse import ArgumentParser

# Get the path to the verif sim dir, this script lives in it
verif_sim_dir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))

class RangeInput:
    '''
    Each instance of this class corresponds to one line of data in the unit test data file.
    This is intended as a container for the inputs to SIM_unit_test, specifically the data
    held in Unit_test_data/*. This data is then fed to the calc_ functions here to provide
    independent verification of the result. The results of the computations are also stored
    in each instance in the outputs dictionary
    '''
    def __init__(self):
        self.data = {}        # Contains the data read from the data files
        self.index = -1       # Corresponds to row in input data file (myArgs.data)
        self.reference = {}  
        self.reference['to'] = {}
        self.reference['from'] = {}
        # Reference planet-fixed state for range_to_pfix  (moving source to fixed target) [uses cartesian]
        self.reference['to']['latitude']  = None   
        self.reference['to']['longitude'] = None   
        self.reference['to']['altitude']  = None   
        self.reference['to']['azimuth']   = None    # NOT USED, but here for completeness
        # Reference planet-fixed state for range_from_pfix  (fixed source to moving target) [uses polar]
        self.reference['from']['latitude']  = None   
        self.reference['from']['longitude'] = None   
        self.reference['from']['altitude']  = None   
        self.reference['from']['azimuth']   = None  # Heading angle of reference point (radians east from north) [alpha]
        # Vehicle planet-fixed state
        self.latitude  = None   
        self.longitude = None   
        self.altitude  = None   
        self.direction = None  # Vector direction of vehicle, used as azimuth for range_to_pfix, not used in range_from_pfix
        self.radius    = None  # Only used for computing arclengths
        # Outputs
        self.outputs = {}
        self.outputs['to'] = {}
        self.outputs['from'] = {}

        for which in self.outputs:
            self.outputs[which]['totalrange_angle']   = self.outputs[which]['downrange_angle']    = self.outputs[which]['crossrange_angle']  = None   
            self.outputs[which]['totalrange_avg_rad'] = self.outputs[which]['crossrange_avg_rad'] = self.outputs[which]['downrange_avg_rad'] = None   
            self.outputs[which]['totalrange_ref_rad']   = self.outputs[which]['crossrange_ref_rad'] = self.outputs[which]['downrange_ref_rad'] = None   

    def initialize(self):
        '''
        Take the internal data values and populate generically named members
        You might ask, why not operate on info in data directly? I like this
        approach because it keeps the variable.path.names and their associated
        python-side variable counterparts in one place.  If the naming ever 
        changes on the sim side, we can change it in one place and be done.
        '''

        self.reference['to']['latitude']  = self.data['test.range_to_pfix.reference_data.latitude']
        self.reference['to']['longitude'] = self.data['test.range_to_pfix.reference_data.longitude']
        self.reference['to']['altitude']  = self.data['test.range_to_pfix.reference_data.altitude']
        self.reference['to']['azimuth']   = self.data['test.range_to_pfix.reference_data.azimuth']

        self.reference['from']['latitude']  = self.data['test.range_from_pfix.reference_data.latitude']
        self.reference['from']['longitude'] = self.data['test.range_from_pfix.reference_data.longitude']
        self.reference['from']['altitude']  = self.data['test.range_from_pfix.reference_data.altitude']
        self.reference['from']['azimuth']   = self.data['test.range_from_pfix.reference_data.azimuth']

        self.latitude  = self.data['test.vehicle_position.ellip_coords.latitude']
        self.longitude = self.data['test.vehicle_position.ellip_coords.longitude']
        self.altitude  = self.data['test.vehicle_position.ellip_coords.altitude']
        self.direction = np.array( [self.data['test.pfix_rel_vel[0]'], self.data['test.pfix_rel_vel[1]'], self.data['test.pfix_rel_vel[2]'] ] )

    def compute_arclengths(self):
        '''
        Using the radius values and range angles, populate the outputs dict with arclengths
        '''
        for which in self.outputs:
            if (self.outputs[which]['totalrange_angle'] == None or
                self.outputs[which]['downrange_angle']  == None or
                self.outputs[which]['crossrange_angle'] == None or
                self.outputs[which]['radius']           == None  ) :
                print("ERROR: Can't compute arclength values if range angles & radii aren't defined yet.")
                sys.exit(1)

        self.outputs
        for which in self.outputs:
             avg_radius = (self.outputs[which]['radius'] + self.radius) / 2.0

             self.outputs[which]['totalrange_ref_rad'] = self.outputs[which]['totalrange_angle'] * self.outputs[which]['radius']
             self.outputs[which]['crossrange_ref_rad'] = self.outputs[which]['crossrange_angle'] * self.outputs[which]['radius']
             self.outputs[which]['downrange_ref_rad']  = self.outputs[which]['downrange_angle']  * self.outputs[which]['radius']

             self.outputs[which]['totalrange_avg_rad'] = self.outputs[which]['totalrange_angle'] * avg_radius
             self.outputs[which]['crossrange_avg_rad'] = self.outputs[which]['crossrange_angle'] * avg_radius
             self.outputs[which]['downrange_avg_rad']  = self.outputs[which]['downrange_angle']  * avg_radius

    def report(self):
        '''
        Helper method for inspecting inputs and outputs
        '''
        print( 72*'-')
        print( '%s Reporting:' % self.__class__.__name__  )
        print( 72*'-')
        print( ' idx      : %d' % self.index  )
        print( ' Reference Data:')
        for ref in self.reference:
            for item in self.reference[ref]:
                print( '  (%-5s) %-20s : %f' % (ref, item, self.reference[ref][item] ) )
        print( ' Vehicle Data:')
        print( '  latitude : %-20f' % self.latitude  )
        print( '  longitude: %-20f' % self.longitude )
        print( '  altitude : %-20f' % self.altitude  )
        print( '  radius :   %-20f' % self.radius  )
        print( '  direction: %s' % str(self.direction ))
        print( ' Output Data:')
        for ref in self.outputs:
            for item in self.outputs[ref]:
                print('  ({0:<4}) {1:<30} : {2:< 20.10}'.format(ref, item, self.outputs[ref][item]))


def getArgs():
    """
    """
    parser = ArgumentParser(description='Provides independent verification of the Range model by reading Unit_test_data/* files and doing the mathematical computations to calculate the total range, cross range, and downrange angles and arclengths.  Outputs from this script can be compared directly to the outputs of SIM_unit_tests/RUN_unit_test/*.csv, which should provide the same result within numerical precision.')
    parser.add_argument( '-v', "--variables", default=os.path.join(verif_sim_dir,'Unit_test_data/variables.txt'), help="Path to input variables file." )
    parser.add_argument( '-d', "--data", default=os.path.join(verif_sim_dir,'Unit_test_data/data.txt'), help="Path to input data file." )
    myArgs = parser.parse_args()

    return myArgs



def read_data(vars_file, data_file):
    '''
    This method is written with the assumptions of the unit_test framework, most 
    notably the order of variables.  Reads the vars file and data file and stores 
    the information in a list of RangeInput instances, returns the list.
    '''
    inputs = []  
    lines = 0
    variables = []
    with open(vars_file, 'r') as f:
        for line in f:
            variables.append(line.strip())
            lines += 1
    num_vars = lines

    idx = 0
    with open(data_file, 'r') as f:
        for line in f:
            input = RangeInput()
            vals = line.strip().split()
            if len(vals) != num_vars:
                print("ERROR: Mismatch in columns and rows in given files: \n  %s and \n  %s \nexiting." % (vars_file, data_file))
                sys.exit(1)
            input.index = idx
            vidx=0
            for val in vals:
                input.data[variables[vidx]] = float(val)
                vidx += 1
            input.initialize()
            inputs.append(input)
            idx += 1

    return inputs

def calc_polar(phi_p, phi_v, lambda_p, lambda_v, alpha):
    '''
    Calculate the range values using the polar method.  This is based on Section 4.1
    of the Range document
      IN   phi_p:        latitude of reference point P
      IN   phi_v:        latitude of point V
      IN   lambda_p:     longitude of reference point P
      IN   lambda_v:     longitude of point V
      IN   alpha:        reference point azimuth at point P (radians east from north)
      CALC lambda_diff:  difference in longitude between point P & V
      CALC beta:         heading angle, angle between reference circle (P) and great circle passing through P & V
      OUT  tau:          total range between P & V
      OUT  xi:           cross range between P & V, [alpha + beta = theta]
      OUT  delta:        down  range between P & V
    '''
    lambda_diff = lambda_v - lambda_p
    tau         = np.arccos( bounds_protect(np.sin(phi_p)*np.sin(phi_v) + np.cos(phi_p)*np.cos(phi_v)*np.cos(lambda_diff)) )

    S = np.cos(phi_v) * np.sin(lambda_diff);  # sin(theta) * sin(tau)
    C = np.sin(phi_v) * np.cos(phi_p) - np.cos(phi_v) * np.sin(phi_p) * np.cos(lambda_diff)  # cos(theta) * sin(tau)

    xi          = np.arcsin( bounds_protect(S * np.cos(alpha) - C * np.sin(alpha)))
    delta       = np.arctan2( C * np.cos(alpha) + S * np.sin(alpha), np.cos(tau) )
    return ( (tau, xi, delta) )


# Points P & V are "arbitrary", meaning which one is the the point of the moving vehicle depends on 
# whether you are implementing RangeFrom / RangeTo.  r_p should be the point whose azimuth is
# specified by direction vector p_direction.   The naming convention here is based on the 
# Range mathematical formulation in Section 4.2 of the documentation.  
def calc_cartesian(r_p, r_v, p_direction):
    '''
    Calculate the range values using the cartesian method. This is based on Section 4.2 
    of the Range document
      IN   r_p:          vector from planet center to point P
      IN   r_v:          vector from planet center to point V
      IN   p_direction:  direction vector at point P
      OUT  tau:          total range between P & V
      OUT  xi:           cross range between P & V, [alpha + beta = theta]
      OUT  delta:        down  range between P & V
    '''
    r_v = np.array(r_v)
    r_p = np.array(r_p)
    p_direction = np.array(p_direction)
    r_v_hat = norm_vector(r_v)
    r_p_hat = norm_vector(r_p)
    tau = np.arccos(bounds_protect(np.dot(r_p_hat, r_v_hat)))
    a = np.cross(r_v, r_p)
    b = np.cross(p_direction, r_p)
    b_hat = norm_vector(b)  # HERE's where we divide by zero, need this to produce [ 0 0 0 ] vec
    xi = np.arcsin( bounds_protect(np.dot(r_v_hat, b_hat)) )

    # Protect for divide by zero
    if np.cos(xi) == 0.0:
       delta = 0.0
    else:
       delta = np.arccos( bounds_protect(np.cos(tau) / np.cos(xi)) ) 
       if np.dot(a, b) < 0.0:
           delta = -delta

    return ( (tau, xi, delta) )

# Normalize the incoming vector, returning zero vector if magnitude is zero, unit vector otherwise
def norm_vector(vec):
    mag = np.linalg.norm(vec)
    if mag <= 0.0:
        return np.array([0.0, 0.0, 0.0])
    else:
        return vec/mag


# Boundary protection for inverse sin/cos due to numerical precision exceeding bounds
def bounds_protect(val):
    if val < -1.0:
        return -1.0
    if val > 1.0:
        return 1.0
    return val

# Adapted from algorithm here: http://danceswithcode.net/engineeringnotes/geodetic_to_ecef/geodetic_to_ecef.html
# Using an independent calculation for [elliptical] lat/long/alt -> ECEF  was chosen since there's no good way to 
# leverage JEOD's lat/long/alt -> PFIX conversion from this stand-alone verif script
def geo_to_ecef (lat, lon, alt):
    '''
    Convert Lat, Lon, Altitude to Earth-Centered-Earth-Fixed (ECEF)
    Input is a three element array containing lat, lon (rads) and alt (m)
    Returned array contains x, y, z in meters
    '''
    a = 6378137.0                 # WGS-84 semi-major axis
    e2 = 6.6943799901377997e-3    # WGS-84 first eccentricity squared
    ecef = np.array([0.0, 0.0, 0.0])
    n = a/np.sqrt( 1 - e2*np.sin( lat )*np.sin( lat ) ) 
    ecef[0] = ( n + alt )*np.cos( lat )*np.cos( lon )     # ECEF x
    ecef[1] = ( n + alt )*np.cos( lat )*np.sin( lon )     # ECEF y
    ecef[2] = ( n*(1 - e2 ) + alt )*np.sin( lat )         # ECEF z

    return( ecef )     

def ellip_to_sphere (lat, lon, alt):
    (x, y, z) = ellip_to_cart(lat, lon, alt)
    (lat, lon, alt) = cart_to_sphere(x, y, z)
    return (lat, lon, alt)

def sphere_to_ellip (lat, lon, alt):
    (x, y, z) = spher_to_cart(lat, lon, alt)
    (lat, lon, alt) = cart_to_ellip(x, y, z)
    return (lat, lon, alt)

# Algorithm adopted from jeod's utils/planet_fixed/planet_fixed_posn/src/planet_fixed_posn.cc
def ellip_to_cart (lat, lon, alt):
    e = 0.081819221                # Planet's ellipsoid eccentricity
    radius_eq = 1000 * (6378.137)  # Equatorial radius
    sin_lat = np.sin(lat)
    cos_lat = np.cos(lat)
    rc_ellipse = radius_eq / ( np.sqrt( 1.0 - (  (e*e) * sin_lat * sin_lat) ) ) 
    x_ellipse = (rc_ellipse + alt) * cos_lat 
    x = x_ellipse * np.cos (lon) 
    y = x_ellipse * np.sin (lon) 
    z = ((rc_ellipse * (1.0 - e*e)) + alt) * sin_lat 
    return (x, y, z)
   
# Algorithm adopted from jeod's utils/planet_fixed/planet_fixed_posn/src/planet_fixed_posn.cc
def spher_to_cart(lat, long, alt):
    radius_eq = 1000 * (6378.137)  # Equatorial radius
    sin_lat = np.sin (lat)
    cos_lat = np.cos (lat)
 
    sin_lon = np.sin (long)
    cos_lon = np.cos (long)
 
    radius = radius_eq + alt
    x = radius * cos_lat * cos_lon
    y = radius * cos_lat * sin_lon
    z = radius * sin_lat

    return (x, y, z)

# Algorithm adopted from jeod's utils/planet_fixed/planet_fixed_posn/src/planet_fixed_posn.cc
def cart_to_sphere (x, y, z):
    r_eq = 1000 * (6378.137)  # Equatorial radius
    r_local = np.linalg.norm(np.array([x, y, z]));
    if r_local < 1.0:
        print("ERROR! Too close to center of earth! Can't calculate spherical coords")
        sys.exit (1)
    lat = np.arcsin (z / r_local);
    lon = np.arctan2 (y, x);
    alt = r_local - r_eq;
    return (lat, lon, alt)


# Algorithm adopted from jeod's utils/planet_fixed/planet_fixed_posn/src/planet_fixed_posn.cc
def cart_to_ellip(x, y, z):
    r_eq = 1000 * (6378.137)  # Equatorial radius
    x_ellipse_sq = (x * x) + (y*y)
    x_ellipse = np.sqrt (x_ellipse_sq);
 
    z_ellipse = z
    z_ellipse_sq = z_ellipse * z_ellipse
 
    r_ellipse_sq = x_ellipse_sq + z_ellipse_sq;
    r_ellipse    = np.sqrt (r_ellipse_sq);
 
    if (r_ellipse < 100) :  # small radius limit
        print("ERROR: Small radius limit reached in cart_to_ellip")
        return (0, 0, 0)
 
    (lat, alt) = get_elliptic_parameters(x_ellipse, z_ellipse)
    long = np.arctan2 (y, x);
    return (lat, long, alt)
 

# Algorithm adopted from jeod's utils/planet_fixed/planet_fixed_posn/src/planet_fixed_posn.cc
def get_elliptic_parameters(r, z):
   r_eq = 1000 * (6378.137)  # Equatorial radius
   a=r_eq;
   e_ellip_sq =  0.006694384925
   flat_coeff = 1.0 - np.sqrt (1.0 - e_ellip_sq);
   r_pol = r_eq * (1.0 - flat_coeff);
   b = r_pol
   y = 0

   if (r > 0.0) :
      y0=np.arctan(a*z/(b*r))
      ar=a*r
      bz=b*z
      w=np.arctan(bz/ar)
      c=(a*a-b*b)/np.sqrt(ar*ar+bz*bz)
      d = 0.0

      for i in range(0,10): 
         d = 2.0*(np.cos(y0-w) - c*np.cos(2.0*y0));
         y = y0 - (2.0*np.sin(y0-w)-c*np.sin(2.0*y0))/d;
         if (np.fabs(y-y0) < 1.0e-12):
            break
         y0 = y;
      lat = np.arctan(a*np.sin(y)/(b*np.cos(y)));
   else :
      y = lat = 0.5*z*np.pi/np.fabs(z);

   alt = (r-a*np.cos(y))*np.cos(lat) + (z-b*np.sin(y))*np.sin(lat);

   return (lat, alt)

def main():
    """
    """
    myArgs = getArgs()
    inputs = read_data(myArgs.variables, myArgs.data)
    for input in inputs:
        # Convert the lat/long/alt from their input elliptical form to the equivalent
        # spherical lat/long/alt
        (sphere_lat, sphere_long, sphere_alt) = ellip_to_sphere(input.latitude, input.longitude, input.altitude)
        # calc radius of vehicle point so we can compute arclengths later
        input.radius = np.linalg.norm(geo_to_ecef (input.latitude,
                                                   input.longitude,
                                                   input.altitude))

        # Calculate total, cross, and downrange values using the polar method (RangeFrom..)
        (tau, xi, delta ) =  calc_polar(input.reference['from']['latitude'], 
                                        sphere_lat,
                                        input.reference['from']['longitude'], 
                                        sphere_long,
                                        input.reference['from']['azimuth'])
        # Store the outputs in the class member dict
        input.outputs['from']['totalrange_angle'] = tau
        input.outputs['from']['crossrange_angle'] = xi
        input.outputs['from']['downrange_angle']  = delta

        # reference data comes in as spherical, we need to convert it to elliptical before sending
        # it into geo_to_ecef!!
        from_lat, from_long, from_alt = sphere_to_ellip(input.reference['from']['latitude'],
                                                        input.reference['from']['longitude'],
                                                        input.reference['from']['altitude'])
        to_lat, to_long, to_alt       = sphere_to_ellip(input.reference['to']['latitude'],
                                                        input.reference['to']['longitude'],
                                                        input.reference['to']['altitude'])

        # calc radius of reference point so we can compute arclengths later
        input.outputs['from']['radius'] = np.linalg.norm(geo_to_ecef (from_lat, from_long, from_alt))


        # Calculate the ECEF position of the reference point
        r_reference_point = geo_to_ecef(to_lat, to_long, to_alt)

        # Calculate the ECEF position of the vehicle (inputs are elliptical so no conversion necessary)
        r_vehicle = geo_to_ecef(input.latitude,
                          input.longitude,
                          input.altitude)

        # Calculate total, cross, and downrange values using the cartesian method (RangeTo..)
        (tau, xi, delta ) =  calc_cartesian(r_vehicle, r_reference_point, input.direction)

        # Store the outputs in the class member dict
        input.outputs['to']['totalrange_angle'] = tau
        input.outputs['to']['crossrange_angle'] = xi
        input.outputs['to']['downrange_angle']  = delta
        # calc radius of reference point so we can compute arclengths later
        input.outputs['to']['radius'] = np.linalg.norm(geo_to_ecef (to_lat, to_long, to_alt))
    
        input.compute_arclengths()
        input.report()

    return 0

if __name__ == "__main__": 
    sys.exit(main()) 
