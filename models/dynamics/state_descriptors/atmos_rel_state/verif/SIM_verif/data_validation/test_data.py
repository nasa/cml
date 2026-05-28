import math

# This code duplicates the functionality of the AtmosRelState model for
# verification runs 2 and 3. The purpose is to produce data in a more
# mathemathical manner to compare with the verification data to help
# verify the data

# Class Vector
# Duplicates needed functionality of the JEOD Vector3 class
# for this test
class Vector :

  # adds vectors v1 and v2, and returns the results in v_out
  @classmethod
  def add( cls, v1, v2, v_out ) :
    v_out[0] = v1[0] + v2[0]
    v_out[1] = v1[1] + v2[1]
    v_out[2] = v1[2] + v2[2]


  # subtracts vector v2 from vecto v1, and returns the results in v_out
  @classmethod
  def diff( cls, v1, v2, v_out ) :
    v_out[0] = v1[0] - v2[0]
    v_out[1] = v1[1] - v2[1]
    v_out[2] = v1[2] - v2[2]


  # multiplies vector v1 by the float val, and returns the result in v_out
  @classmethod
  def mul( cls, v1, val, v_out ) :
    v_out[0] = v1[0] * val
    v_out[1] = v1[1] * val
    v_out[2] = v1[2] * val


  # returns the magnitude of vector v
  @classmethod
  def mag( cls, v ) :
    return math.sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] )


  # normalizes vector v_in, and returns the result in vector v_out
  @classmethod
  def norm( cls, v_in, v_out ) :
    mag = Vector.mag( v_in )
    if mag > 0.0 :
      Vector.mul( v_in, 1.0 / mag, v_out )
    else :
      v_out = Vector.empty()


  # calculates the cross product of vectors v1 and v2, and returns the
  # result in v_out
  @classmethod
  def cross( cls, v1, v2, v_out ) :
   v_out[0] = v1[1] * v2[2] - v1[2] * v2[1]
   v_out[1] = v1[2] * v2[0] - v1[0] * v2[2]
   v_out[2] = v1[0] * v2[1] - v1[1] * v2[0]


  # returns the product of matrix m and vector v_in in vector v_out
  @classmethod
  def transform( cls, m, v_in, v_out ) :
    v_out[0] = m[0][0] * v_in[0] + m[0][1] * v_in[1] + m[0][2] * v_in[2]

    v_out[1] = m[1][0] * v_in[0] + m[1][1] * v_in[1] + m[1][2] * v_in[2]

    v_out[2] = m[2][0] * v_in[0] + m[2][1] * v_in[1] + m[2][2] * v_in[2]


  #returns a vector with all values 0
  @classmethod
  def empty( cls ) :
    vector = [0.0, 0.0, 0.0]
    return vector



# Class Matrix
# Duplicates needed functionality of the JEOD Matrix3x3 class
# for this test
class Matrix :

  # returns the determinant of matrix m
  @classmethod
  def determinant( cls, m ) :
    return m[0][0] * ( m[1][1] * m[2][2] - m[1][2] * m[2][1] ) - \
           m[0][1] * ( m[1][0] * m[2][2] - m[1][2] * m[2][0] ) + \
           m[0][2] * ( m[1][0] * m[2][1] - m[1][1] * m[2][0] )


  # normalizes the matrix m
  @classmethod
  def normalize( cls, m ) :
    determinant = Matrix.determinant( m )
    for row in range(2) :
      for col in range(2) :
        m[row][col] = m[row][col] / determinant


  # calculates the right transpose of matrices m1 and m2, and returns the
  # result in matrix m_out
  @classmethod
  def product_right_transpose( cls, m1, m2, m_out ) :
    m_out[0][0] = m1[0][0] * m2[0][0] + \
                  m1[0][1] * m2[0][1] + \
                  m1[0][2] * m2[0][2]

    m_out[0][1] = m1[0][0] * m2[1][0] + \
                  m1[0][1] * m2[1][1] + \
                  m1[0][2] * m2[1][2]

    m_out[0][2] = m1[0][0] * m2[2][0] + \
                  m1[0][1] * m2[2][1] + \
                  m1[0][2] * m2[2][2]

    m_out[1][0] = m1[1][0] * m2[0][0] + \
                  m1[1][1] * m2[0][1] + \
                  m1[1][2] * m2[0][2]

    m_out[1][1] = m1[1][0] * m2[1][0] + \
                  m1[1][1] * m2[1][1] + \
                  m1[1][2] * m2[1][2]

    m_out[1][2] = m1[1][0] * m2[2][0] + \
                  m1[1][1] * m2[2][1] + \
                  m1[1][2] * m2[2][2]

    m_out[2][0] = m1[2][0] * m2[0][0] + \
                  m1[2][1] * m2[0][1] + \
                  m1[2][2] * m2[0][2]

    m_out[2][1] = m1[2][0] * m2[1][0] + \
                  m1[2][1] * m2[1][1] + \
                  m1[2][2] * m2[1][2]

    m_out[2][2] = m1[2][0] * m2[2][0] + \
                  m1[2][1] * m2[2][1] + \
                  m1[2][2] * m2[2][2]


  # multiplies matrices m1 and m2, and returns the result in matrix m_out
  @classmethod
  def product( cls, m1, m2, m_out ) :
    m_out[0][0] = m1[0][0] * m2[0][0] + \
                  m1[0][1] * m2[1][0] + \
                  m1[0][2] * m2[2][0]

    m_out[0][1] = m1[0][0] * m2[0][1] + \
                  m1[0][1] * m2[1][1] + \
                  m1[0][2] * m2[2][1]

    m_out[0][2] = m1[0][0] * m2[0][2] + \
                  m1[0][1] * m2[1][2] + \
                  m1[0][2] * m2[2][2]

    m_out[1][0] = m1[1][0] * m2[0][0] + \
                  m1[1][1] * m2[1][0] + \
                  m1[1][2] * m2[2][0]

    m_out[1][1] = m1[1][0] * m2[0][1] + \
                  m1[1][1] * m2[1][1] + \
                  m1[1][2] * m2[2][1]

    m_out[1][2] = m1[1][0] * m2[0][2] + \
                  m1[1][1] * m2[1][2] + \
                  m1[1][2] * m2[2][2]

    m_out[2][0] = m1[2][0] * m2[0][0] + \
                  m1[2][1] * m2[1][0] + \
                  m1[2][2] * m2[2][0]

    m_out[2][1] = m1[2][0] * m2[0][1] + \
                  m1[2][1] * m2[1][1] + \
                  m1[2][2] * m2[2][1]

    m_out[2][2] = m1[2][0] * m2[0][2] + \
                  m1[2][1] * m2[1][2] + \
                  m1[2][2] * m2[2][2]


  # returns a matrix filled with zeroes
  @classmethod
  def empty( cls ) :
    return [[0.0, 0.0, 0.0],[0.0, 0.0, 0.0],[0.0, 0.0, 0.0]]


  # copies the matrix m1 into matrix m2
  @classmethod
  def copy( cls, m1, m2 ) :
    for row in range(2) :
      for col in range(2) :
        m2[row][col] = m1[row][col]


# Class AtmosRelState
# The functional equivalent of the Antares model AtmosRelState
class AtmosRelState :

  # Values that are input from outside the class, set with their
  # initial values
  planet_rel_vel  = [ 120.0, 20.0, 0.0 ]
  wind_vel_eci    = [ 100.0, 0.0, 0.0 ]
  state_trans     = [ 5000000.0, 2000000.0, 0.0 ]
  state_rot       = [ [ 0.0 ,1.0 ,0.0 ],
                      [ 0.0 ,0.0 ,1.0 ],
                      [ 1.0 ,0.0 ,0.0 ] ]
  non_grav_accel  = [ 0.02, 0.003, 0.0004 ]
  atmos_density   = 2.0e-8
  speed_of_sound  = 300.0
  topodetic       = [ [ 0.0, 1.0, 0.0 ],
                      [-1.0, 0.0, 0.0 ],
                      [ 0.0, 0.0, 1.0 ] ]

  # Values for reading datafile
  data_file_name  = "../Unit_test_data/data.txt"
  prv0_index      = 6
  prv1_index      = 7
  prv2_index      = 8
  st0_index       = 0
  st1_index       = 1
  st2_index       = 2


  # Values that are internal to the class
  fs_vel_mag      = 0.0
  fs_vel_unit     = Vector.empty()
  f_in            = 0
  f_out           = 0


  # Values computed by the model and output
  free_stream_vel        = Vector.empty()
  free_stream_td_vel     = Vector.empty()
  mach                   = 0.0
  dynamic_pressure       = 0.0
  y_axis                 = Vector.empty()
  y_axis_unit            = Vector.empty()
  z_axis                 = Vector.empty()
  z_axis_unit            = Vector.empty()
  T_inertial_stab        = Matrix.empty()
  T_inertial_traj        = Matrix.empty()
  T_inertial_wind        = Matrix.empty()
  T_traj_body            = Matrix.empty()
  T_traj_wind            = Matrix.empty()
  T_wind_stab            = Matrix.empty()
  traj_to_body_RYP       = Vector.empty()
  bank_angle             = 0.0
  sideslip_angle         = 0.0
  attack_angle           = 0.0
  total_attack_angle     = 0.0
  phi_roll               = 0.0
  fs_flight_path         = 0.0
  fs_azimuth             = 0.0
  drag_accel             = 0.0
  side_accel             = 0.0
  lift_accel             = 0.0
  sensed_accel_mag       = 0.0
  fs_body_vel            = Vector.empty()
  total_attack_angle_alt = 0.0
  attack_angle_alt       = 0.0
  sideslip_angle_alt     = 0.0
  phi_roll_alt           = 0.0
  qalpha                 = 0.0
  qalpha_total           = 0.0
  qalpha_totalpsfdeg     = 0.0
  qbeta                  = 0.0
  reynolds_number        = 0.0


  # calculate the free stream velocity, the velocity magnitute, and the
  # velocity unit vector
  def calc_free_stream_velocity( self ) :
    Vector.diff( self.planet_rel_vel, self.wind_vel_eci, self.free_stream_vel )
    self.fs_vel_mag = Vector.mag( self.free_stream_vel )
    Vector.norm( self.free_stream_vel, self.fs_vel_unit )


  # calculate mach speed
  def calc_mach( self ) :
    self.mach = self.fs_vel_mag / self.speed_of_sound


  # calculates dynamic pressure
  def calc_dynamic_pressure( self ) :
    self.dynamic_pressure = 0.5 * self.atmos_density * self.fs_vel_mag * \
      self.fs_vel_mag


  # calculates the inertial frame to traj frame transformation matrix
  def calc_T_inertial_traj( self ) :
    Vector.cross( self.free_stream_vel, self.state_trans, self.y_axis )
    Vector.norm( self.y_axis, self.y_axis_unit )

    Vector.cross( self.fs_vel_unit, self.y_axis_unit, self.z_axis )
    Vector.norm( self.z_axis, self.z_axis_unit )

    self.T_inertial_traj[0][0] = self.fs_vel_unit[0]
    self.T_inertial_traj[0][1] = self.fs_vel_unit[1]
    self.T_inertial_traj[0][2] = self.fs_vel_unit[2]
    self.T_inertial_traj[1][0] = self.y_axis_unit[0]
    self.T_inertial_traj[1][1] = self.y_axis_unit[1]
    self.T_inertial_traj[1][2] = self.y_axis_unit[2]
    self.T_inertial_traj[2][0] = self.z_axis_unit[0]
    self.T_inertial_traj[2][1] = self.z_axis_unit[1]
    self.T_inertial_traj[2][2] = self.z_axis_unit[2]


  # calculates the trajsctory to body frame transformation matrix
  def calc_T_traj_body( self ) :
    Matrix.product_right_transpose( self.state_rot,
      self.T_inertial_traj, self.T_traj_body )


  # duplicates Orientation.matrix_to_euler_angles for roll/yaw/pitch
  def calc_euler_angles( self ) :
    phi   = 0.0       # First Euler angle
    theta = 0.0     # Second Euler angle
    psi   = 0.0       # Third Euler angle

    cos_phi        = 0.0   # cos(phi) * extra stuff * sign
    sin_phi        = 0.0   # sin(phi) * extra stuff * sign
    cos_psi        = 0.0   # cos(psi) * extra stuff * sign
    sin_psi        = 0.0   # sin(psi) * extra stuff * sign
    theta_val      = 0.0 # sin(theta), -sin(theta) or cos(theta)
    alt_theta_val1 = 0.0 # Conceptually, 1-theta_val^2
    alt_theta_val2 = 0.0 # Conceptually, 1-theta_val^2
    alt_theta_val  = 0.0  # Conceptually, 1-theta_val^2


    # Extract the key elements from the matrix assuming that this is not
    # a gimbal lock situation.
    # The T_traj_body[1][0] element is
    #  *  sin(theta) for even permutation aerodynamics sequences,
    #  * -sin(theta) for odd permutation aerodynamics sequences, or
    #  *  cos(theta) for all astronomical sequences.
    theta_val = self.T_traj_body[1][0]

    # Get terms containing the sines and cosines of the first and third Euler
    # angle times sin(theta) or cos(theta) (and sometimes negated).
    sin_phi = self.T_traj_body[1][2]
    cos_phi = self.T_traj_body[1][1]
    sin_psi = self.T_traj_body[2][0]
    cos_psi = self.T_traj_body[0][0]

    # Compute alternative theta values based on the above four terms.
    alt_theta_val1 = math.sqrt (sin_phi*sin_phi + cos_phi*cos_phi)
    alt_theta_val2 = math.sqrt (sin_psi*sin_psi + cos_psi*cos_psi)
    alt_theta_val  = 0.5 * (alt_theta_val1 + alt_theta_val2)

    # theta_val is -sin(theta) for odd permutation aerodynamics sequences.
    # Negate to get rid of the minus sign.
    theta_val = -theta_val

    # Compute theta.
    if (alt_theta_val < abs (theta_val)) :
      alt_theta = math.asin (alt_theta_val)

      if (theta_val < 0.0) :
        theta = -0.5*math.pi + alt_theta
      else :
        theta =  0.5*math.pi - alt_theta
    else :
      theta = math.asin (theta_val)

    # Not in a gimbal lock situation :
    # Compute sin_phi, cos_phi, sin_psi, and cos_psi.
    #  - These are not the sine and cosine of the Euler angles phi and psi.
    #    Rather, they are sin(phi) etc. scaled by a common positive number.
    #  - Key elements of the matrix are of the form
    #      sign*cos(theta)*sin(phi) etc.
    #  - The trick then is to find these key elements and then ensure that
    #    sign*cos(theta) is positive for each of the four values.
    #  - The specifics of the sign correction are depend on whether the
    #    sequence is an aerodynamical or astronomical sequence.
    if (alt_theta_val > 1e-13) :

      # Correct signs for aerodynamic sequences.

      # Compute phi and psi.
      phi = math.atan2 (sin_phi, cos_phi)
      psi = math.atan2 (sin_psi, cos_psi)


    # In a gimbal lock situation :
    # All that can be determine is the difference between / sum of phi and psi.
    # Arbitrarily setting psi to zero resolves this issue.
    else :
      # Compute sin_phi and cos_phi with the same constraint on the
      # common scale factor as outlined above.
      sin_phi = self.T_traj_body[2][1]
      cos_phi = self.T_traj_body[2][2]

      # The sine value has the wrong sign for odd sequences.
      sin_phi = -sin_phi

      # Compute phi and psi.
      phi = math.atan2 (sin_phi, cos_phi)
      psi = 0.0

    # Save the computed angles in the object.
    self.traj_to_body_RYP[0] = phi
    self.traj_to_body_RYP[1] = theta
    self.traj_to_body_RYP[2] = psi


  # duplicates the functionality of the angle calculations in the 
  # Antares model, which is a subscribable functionality
  def calc_angles( self ) :
    self.calc_T_inertial_traj()
    self.calc_T_traj_body()
    self.calc_euler_angles()

    self.bank_angle     =  self.traj_to_body_RYP[0]
    self.sideslip_angle = -self.traj_to_body_RYP[1]
    self.attack_angle   =  self.traj_to_body_RYP[2]

    self.total_attack_angle = math.acos(
      math.cos( self.attack_angle ) * math.cos( self.sideslip_angle ) )
    self.phi_roll = math.atan2(
      math.tan( self.sideslip_angle ), math.sin( self.attack_angle ) )


  # calculates the free stream flight path and azimuth
  def calc_fpangle_azimuth( self ) :
    Vector.transform(
      self.topodetic, self.free_stream_vel, self.free_stream_td_vel )

    vel_xy = math.sqrt(
      self.free_stream_td_vel[0] * self.free_stream_td_vel[0] +
      self.free_stream_td_vel[1] * self.free_stream_td_vel[1] )

    self.fs_flight_path = math.atan2( -self.free_stream_td_vel[2] , vel_xy )
    self.fs_azimuth     = math.atan2( self.free_stream_td_vel[1],
                                      self.free_stream_td_vel[0] )


  # creates a transformation matrix from a roll/yaw/pitch rotation
  def calc_from_eigen_rotation( self, angle, axis, trans ) :
    cphi = math.cos( angle )
    sphi = math.sin( angle )
    omcp = 1.0 - cphi

    trans[0][0] = cphi + omcp * axis[0] * axis[0]
    trans[1][1] = cphi + omcp * axis[1] * axis[1]
    trans[2][2] = cphi + omcp * axis[2] * axis[2]

    trans[0][1] = omcp * axis[0] * axis[1]
    trans[1][0] = trans[0][1]
    trans[0][2] = omcp * axis[0] * axis[2]
    trans[2][0] = trans[0][2]
    trans[1][2] = omcp * axis[1] * axis[2]
    trans[2][1] = trans[1][2]

    delta = sphi * axis[2]
    trans[0][1] += delta
    trans[1][0] -= delta

    delta = sphi * axis[1]
    trans[2][0] += delta
    trans[0][2] -= delta

    delta = sphi * axis[0]
    trans[1][2] += delta
    trans[2][1] -= delta



  # calculates the inertial frame to wind frame transformation matrix, and
  # the inertial frame to wind stability transformation matrix
  def calc_inertial_stab( self ) :
    eigen_axis = [ 1, 0, 0 ]
    self.calc_from_eigen_rotation(
      self.bank_angle, eigen_axis, self.T_traj_wind )

    Matrix.product(
      self.T_traj_wind, self.T_inertial_traj, self.T_inertial_wind )

    eigen_axis = [0, 0, 1]
    self.calc_from_eigen_rotation(
      -self.sideslip_angle, eigen_axis, self.T_wind_stab )

    Matrix.product(
      self.T_wind_stab, self.T_inertial_wind, self.T_inertial_stab )


  # calculates from accelerations from drag, side wind, and lift
  def calc_accel_wind( self ) :
    wind_accel = [ 0.0, 0.0, 0.0 ]
    Vector.transform( self.T_inertial_wind, self.non_grav_accel, wind_accel )

    self.drag_accel = -wind_accel[0]
    self.side_accel =  wind_accel[1]
    self.lift_accel = -wind_accel[2]
    self.sensed_accel_mag = Vector.mag( self.non_grav_accel )


  # calculates alternative values for the attack angle, sideslip angle,
  # total attack angle, and phi roll
  def calc_alternate_angles( self ) :
    Vector.transform( self.state_rot, self.free_stream_vel, self.fs_body_vel )

    fs_body_vel_xz = math.sqrt(
      self.fs_body_vel[0] * self.fs_body_vel[0] +
      self.fs_body_vel[2] * self.fs_body_vel[2] )
    fs_body_vel_yz = math.sqrt(
      self.fs_body_vel[1] * self.fs_body_vel[1] +
      self.fs_body_vel[2] * self.fs_body_vel[2] )

    self.total_attack_angle_alt = math.atan2(
      fs_body_vel_yz, self.fs_body_vel[0] )
    self.attack_angle_alt = math.atan2(
      self.fs_body_vel[2], self.fs_body_vel[0] )
    self.sideslip_angle_alt = math.atan2(
      self.fs_body_vel[1], fs_body_vel_xz )
    self.phi_roll_alt = math.atan2(
      self.fs_body_vel[1], self.fs_body_vel[2] )


  # calculates the dynamic pressure qalpha and qbeta values
  def calc_dyn_pressure_extras( self ) :
    self.qalpha_total = self.dynamic_pressure * \
        ( math.pi/2 - abs(self.total_attack_angle - math.pi/2))
    self.qalpha = self.dynamic_pressure * \
        ( math.pi/2 - abs(self.attack_angle - math.pi/2))

    #convert radians newtons/meter^2 to deg lbs force / foot^2
    rad_to_deg = 57.29577951308231
    lbsf_to_n  = 4.4482216152605
    m_to_f     = 3.28284
    self.qalpha_totalpsfdeg = self.qalpha_total * \
      rad_to_deg /  lbsf_to_n / m_to_f / m_to_f

    self.qbeta  = self.dynamic_pressure * self.sideslip_angle


  # calculate the reynolds number
  def calc_reynolds_number( self ) :
    # 2.0e-5 is atmospheric viscosity
    self.reynolds_number = self.atmos_density * self.fs_vel_mag * 5.0292 / 2.0e-5



  # perform all calculations done when the model is subscribed to
  def calc( self ) :
    self.calc_free_stream_velocity()
    self.calc_mach()
    self.calc_dynamic_pressure()


  # perform all calculations dom when the "complete calculations" is
  # subscribed to
  def calc_complete( self ) :
    self.calc_fpangle_azimuth()
    self.calc_inertial_stab()
    self.calc_accel_wind()
    self.calc_alternate_angles()
    self.calc_dyn_pressure_extras()
    self.calc_reynolds_number()


  # prints an instance of all output data to the output data file
  def print_data( self, seconds ) :
    format_str = "{0:d}"
    for ii in range(1,87):
      format_str = format_str + ",{" + str(ii) + ":20.13f}"
    format_str = format_str + "\n"

    output = format_str.format(
      seconds,
      self.fs_vel_mag,
      self.free_stream_vel[0],
      self.free_stream_vel[1],
      self.free_stream_vel[2],
      self.free_stream_td_vel[0],
      self.free_stream_td_vel[1],
      self.free_stream_td_vel[2],
      self.fs_body_vel[0],
      self.fs_body_vel[1],
      self.fs_body_vel[2],
      self.fs_flight_path,
      self.fs_azimuth,
      self.attack_angle,
      self.sideslip_angle,
      self.bank_angle,
      self.total_attack_angle,
      self.phi_roll,
      self.mach,
      self.dynamic_pressure,
      self.drag_accel,
      self.side_accel,
      self.lift_accel,
      self.sensed_accel_mag,
      self.attack_angle_alt,
      self.sideslip_angle_alt,
      self.total_attack_angle_alt,
      self.phi_roll_alt,
      self.reynolds_number,
      self.qalpha_totalpsfdeg,
      self.qalpha_total,
      self.qalpha,
      self.qbeta,
      self.T_inertial_traj[0][0],
      self.T_inertial_traj[0][1],
      self.T_inertial_traj[0][2],
      self.T_inertial_traj[1][0],
      self.T_inertial_traj[1][1],
      self.T_inertial_traj[1][2],
      self.T_inertial_traj[2][0],
      self.T_inertial_traj[2][1],
      self.T_inertial_traj[2][2],
      self.T_traj_body[0][0],
      self.T_traj_body[0][1],
      self.T_traj_body[0][2],
      self.T_traj_body[1][0],
      self.T_traj_body[1][1],
      self.T_traj_body[1][2],
      self.T_traj_body[2][0],
      self.T_traj_body[2][1],
      self.T_traj_body[2][2],
      self.T_traj_wind[0][0],
      self.T_traj_wind[0][1],
      self.T_traj_wind[0][2],
      self.T_traj_wind[1][0],
      self.T_traj_wind[1][1],
      self.T_traj_wind[1][2],
      self.T_traj_wind[2][0],
      self.T_traj_wind[2][1],
      self.T_traj_wind[2][2],
      self.T_inertial_wind[0][0],
      self.T_inertial_wind[0][1],
      self.T_inertial_wind[0][2],
      self.T_inertial_wind[1][0],
      self.T_inertial_wind[1][1],
      self.T_inertial_wind[1][2],
      self.T_inertial_wind[2][0],
      self.T_inertial_wind[2][1],
      self.T_inertial_wind[2][2],
      self.T_wind_stab[0][0],
      self.T_wind_stab[0][1],
      self.T_wind_stab[0][2],
      self.T_wind_stab[1][0],
      self.T_wind_stab[1][1],
      self.T_wind_stab[1][2],
      self.T_wind_stab[2][0],
      self.T_wind_stab[2][1],
      self.T_wind_stab[2][2],
      self.T_inertial_stab[0][0],
      self.T_inertial_stab[0][1],
      self.T_inertial_stab[0][2],
      self.T_inertial_stab[1][0],
      self.T_inertial_stab[1][1],
      self.T_inertial_stab[1][2],
      self.T_inertial_stab[2][0],
      self.T_inertial_stab[2][1],
      self.T_inertial_stab[2][2] )

    self.f_out.write( output )


  # reads in a set of input data from the input data file. Note that
  # most of the input values don't change, so only the ones below are updated
  def assign_input_data(self, line) :
    parts = line.split()

    self.planet_rel_vel[0] = float( parts[self.prv0_index] )
    self.planet_rel_vel[1] = float( parts[self.prv1_index] )
    self.planet_rel_vel[2] = float( parts[self.prv2_index] )

    self.state_trans[0]    = float( parts[self.st0_index] )
    self.state_trans[1]    = float( parts[self.st1_index] )
    self.state_trans[2]    = float( parts[self.st2_index] )

  # Duplicate the results from RUN_02 of AtmosRelState SIM_verif.
  # This just runs all calculations for 12 seconds.
  def run02( self ) :
    self.f_in  = open( self.data_file_name, "r" )
    self.f_out = open("./RUN_02.csv", "w" )

    index = 0
    for line in self.f_in :
      self.assign_input_data( line )
      self.calc()
      self.calc_angles()
      self.calc_complete()

      self.print_data( index )
      index += 1

    self.f_in.close()
    self.f_out.close()

    print( "RUN02 DONE!" )


  # Duplicates the results of RUN_03 for AtmosRelState SIM_verif. This:
  # 1. starts out calculating all values 
  # 2. unsubscribes the angle calculations at time 3 
  # 3. unsubscribes the "complete" calculations at time 6, and
  # 4. unsubscribes the entire model at time 9
  def run03( self ) :
    self.f_in  = open( self.data_file_name, "r" )
    self.f_out = open( "./RUN_03.csv", "w" )

    index = 0
    for line in self.f_in :
      self.assign_input_data( line )
      if index < 9 :   # all calcs     shut off at time 9
        self.calc()
        if index < 3 : # angle calcs   shut off at time 3
          self.calc_angles()
        if index < 6 : # complete cals shut off at time 6
          self.calc_complete()

      self.print_data( index )
      index += 1

    self.f_in.close()
    self.f_out.close()

    print( "RUN03 DONE!" )


# main
atmos = AtmosRelState()
atmos.run02()
atmos.run03()
