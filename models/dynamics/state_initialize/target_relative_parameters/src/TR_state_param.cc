/*******************************************************************************
PURPOSE:
   (Provide the capability to transform between position/velocity
    vectors and a set of target relative parameters.)

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (Nov 2023)
      (Additional implementation led to a separation of the shared components
       into this baseline class))
   )

*******************************************************************************/

#include <cmath>  /* sin, cos, atan2, abs */
#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh" // MathUtils::*

#include "../include/TR_state_param.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
TargetRelative_StateParam::TargetRelative_StateParam()
  :
  target_is_behind_vehicle(false),
  K_theta(0.0),
  position(),
  velocity(),
  omega_mag(0.0),
  uhat_R{1.0, 0.0, 0.0},
  Target_Reference_frame("Target-Reference frame"),
  Omega_Target_frame("Omega-Target frame"),
  Position_BiasedTarget_frame("InitialPosition-BiasedTarget frame"),
  biased_target_pos_hat(),
  theta_RngBias(0.0),
  sin_phi_omega(0.0),
  cos_phi_omega(0.0),
  flag_initialized(false),
  omega_target_aligned(false),
  k_theta_warning_sent(false),
  target_is_behind_vehicle_check(false)
{}

/*****************************************************************************
compute_pos_vel_from_params
Purpose:(Front-end to compute the velocity and position vectors from a
TR_ParameterSet.)
*****************************************************************************/
void
TargetRelative_StateParam::compute_pos_vel_from_params(
  const  TR_ParameterSet & param_set,
  InputPosAngle            input_theta_type,
  double                   (&R)[3], // out
  double                   (&V_wrt_PCI)[3]) // out
{
  // Copy the contents of param_set into the TR_ParameterSet base-class aspect
  // of this class.
  TR_ParameterSet::operator=( param_set);
  TargetRelative_StateParam::compute_pos_vel_from_params( input_theta_type);
  jeod::Vector3::copy(position, R);
  jeod::Vector3::copy(velocity, V_wrt_PCI);
}
/****************************************************************************/
void
TargetRelative_StateParam::compute_pos_vel_from_params(
  InputPosAngle            input_theta_type)
{
  compute_position_from_params( input_theta_type);
  compute_velocity_from_params();
}


/*****************************************************************************
compute_velocity_from_params
Purpose:
 (Compute the velocity vector w.r.t. the inertial frame and expressed in
  the Planet-Centered Planet-Fixed coordinate system from the inertial
  speed, inertial topocentric flight path angle, lateral angle, and the
  position vector w.r.t. the planet center and expressed in the
  Planet-Centered Planet-Fixed coordinate system.)
Notes:
> For execution from code, passing by reference is available.
  For execution via SWIG, only pass-by-value is available for primitive data
  types (double in this case), so a pass-by-value version is made available.
*****************************************************************************/
void
TargetRelative_StateParam::compute_velocity_from_params_SWIG(
  double        Vmag_,
  double        gamma_,
  double        Lambda_,
  const double  R[3],
  double        V_wrt_PCI[3]) // out
{
  //-----------------------------------------------------
  // Check for improper inputs
  //-----------------------------------------------------
  if (V_wrt_PCI == nullptr) {
    CMLMessage::error( __FILE__,__LINE__,
      "NULL vectors cannot be populated by target-reference parameters.\n"
      "Aborting calculation of velocity vector.\n");
    return;
  }

  if (R == nullptr) {
    CMLMessage::error( __FILE__,__LINE__,
      "Position vector is required to obtain the velocity from parameters.\n"
      "NULL vector passed in for position.\n"
      "Aborting calculation of velocity vector.\n");
    jeod::Vector3::initialize(V_wrt_PCI);
    return;
  }
  jeod::Vector3::copy(R, position);
  Vmag = Vmag_;
  gamma = gamma_;
  Lambda = Lambda_;
  TargetRelative_StateParam::compute_velocity_from_params();
  jeod::Vector3::copy( velocity, V_wrt_PCI);
}
/****************************************************************************/
void
TargetRelative_StateParam::compute_velocity_from_params(
  double        Vmag_,
  double        gamma_,
  double        Lambda_,
  const double  (&R)[3],
  double        (&V_wrt_PCI)[3]) // out
{
  jeod::Vector3::copy(R, position);
  Vmag = Vmag_;
  gamma = gamma_;
  Lambda = Lambda_;
  TargetRelative_StateParam::compute_velocity_from_params();
  jeod::Vector3::copy( velocity, V_wrt_PCI);
}
/****************************************************************************/
void
TargetRelative_StateParam::compute_velocity_from_params()
{
  //-----------------------------------------------------
  // Make sure model has been initialized
  //-----------------------------------------------------
  if (!flag_initialized) {
    CMLMessage::error( __FILE__,__LINE__,
      "Attempt to compute the velocity from assigned parameters before the\n"
      "model has been initialized.\n"
      "Initialization is required to set the u-hat, N-hat and P-hat vectors\n"
      "for the plane defined by the target and omega vectors.\n"
      "Aborting calculation of velocity vector and setting it to [0,0,0].\n");
    jeod::Vector3::initialize(velocity);
    return;
  }

  /* Compute unit position-vector
   * Compute the magnitude of the recorded position
   * If it is zero, try building the position.
   * If it is still zero, nothing we can do.*/
  double r_mag = jeod::Vector3::vmag( position );
  if (MathUtils::is_near_equal( r_mag, 0)) {
    CMLMessage::warn( __FILE__,__LINE__,
      "Trying to construct velocity vector from parameters.\n"
      "The input position has zero magnitude.\n"
      "Attempting to construct a position vector from the parameters.\n");
    compute_position_from_params();
    r_mag = jeod::Vector3::vmag( position );
    if (MathUtils::is_near_equal( r_mag, 0)) {
      CMLMessage::error( __FILE__,__LINE__,
        "The input position still has zero magnitude.\n"
        "Cannot compute the velocity."
        "Setting velocity to zero.\n");
      jeod::Vector3::initialize( velocity);
      return;
    }
  }
  jeod::Vector3::normalize( position,
                      uhat_R);

  //-----------------------------------------------------
  // Compute sine and cosine of flight-path angles
  //-----------------------------------------------------
  double sin_gamma    = std::sin(gamma); // Sin of flight path
  double cos_gamma    = std::cos(gamma); // Cos of flight path

  // Special case - this algorithm will generate the velocity vector based on
  // relative angles between two planes.  In the (extremely unlikely) event
  // that the flight-path angle is +- (pi/2) (i.e. velocity is vertical), the
  // plane defined by the position and velocity vectors is ill-defined and the
  // algorithm has no meaning.  But in such an case, the velocity is trivially
  // obtained instead as vertically downward.:
  if (std::abs(cos_gamma) < epsilon) {
    jeod::Vector3::scale( uhat_R,
                    sin_gamma * Vmag, // +/- V_mag.
                    velocity);
    return;
  }

  //-----------------------------------------------------
  // Compute biased target unit vector
  //-----------------------------------------------------
  if (!compute_biased_target()) {
    // When Position vector and BiasedTarget position vector are nearly
    // aligned, lateral angle is undefined.  It is then not possible
    // to define a velocity vector given a lateral angle value.
    CMLMessage::error( __FILE__,__LINE__,
      "Position vector and BiasedTarget position vector are nearly aligned.\n"
      "In this case, lateral angle is undefined.  It is thus not possible\n"
      "to define a velocity vector relative to the BiasedTarget plane "
      "using lateral angle.\n"
      "Check configuration; setting output velocity to zero.\n");
    jeod::Vector3::initialize(velocity);
    return;
  }

  //-------------------------------------------------------
  // Compute intermediate variables for velocity definition
  //-------------------------------------------------------

  // Compute sine of target heading angle, H
  // target-heading angle is the angle between the position--biased-target
  // plane (defined by the vehicle-position-vector and biased-target-position
  // vectors), and by the position--velocity plane (defined by the
  // vehicle-position vector and vehicle-velocity vector).

  // Note: By definition, the lateral angle (the angle between the
  // biased-target-position vector and its projection onto the position-velocity
  // plane) must be no larger than the biased-range angle (the angle between
  // the biased-target-position vector and the vehicle-position vector) because
  // (obviously) the vehicle-position vector lies in the position-velocity
  // plane.  See documentation Figure 3.4 for illustration.

  // Important note:
  //   theta_RngBias is not close to zero; this has already been checked
  //   in compute_biased_target(...)
  //   Nevertheless, Using divide-protected for safety
  //   By definition, theta_RngBias is in range [0, pi].
  //   Consequently, sin_theta_RngBias is in range [0, 1].
  double sin_H = MathUtils::divide_protected( std::sin(Lambda),
                                              std::sin(theta_RngBias),
                                              2.0,
                                              true);
  // Error if the lateral angle has been specified to an invalid value,
  //   i.e. if |sin_theta_RngBias| < |sin(Lambda)|
  if ( std::abs(sin_H) > 1.0 ) {
    CMLMessage::error(
      __FILE__,__LINE__,
      "Questionable results for inputs Lambda and theta_RngBias!!\n",
      "  Lambda         = ",Lambda,"\n"
      "  theta_RngBias  = ",theta_RngBias,"\n"
      "Results in:\n"
      "  sin_H = 1 + (",sin_H-1,")\n"
      "Adjusting sin_H to +/- 1.0\n\n");
    sin_H = (sin_H > 0)? 1.0 : -1.0;
  }

  // Compute cosine of target heading angle
  // Note: cos(asin(x)) is always positive; taking only the positive
  //       square-root equivalently results in a positive value.*/
  double cos_H = std::sqrt(1 - sin_H*sin_H);

  // For cases where target is behind the position (i.e. vehicle is
  // moving away from the target), the sign of cos(H) is negative.
  // Note that in this case, the value of theta_RngBias is still
  // in range [0, pi] by definition.
  // An extra
  // boolean flag, "target_is_behind_vehicle" (default: false) is provided to
  // support cases where cos(H) should be negative.
  if (target_is_behind_vehicle) {
    cos_H *= -1.0;
  }

  //-----------------------------------------------------
  // Compute unit velocity vector:
  // v-hat =  [cos(gamma), 0, sin(gamma)] in a frame where axes are:
  //  defined by the heading plane (the plane defined by the
  //  vehicle-position vector and vehicle-velocity vector)
  //  [horizontal-in-plane, out-of-plane, vertical-in-plane]
  // that plane is a rotation by heading-angle (H) relative to the
  // biased-target plane (the plane defined by the vehicle-position vector and
  // the biased-targe)t-position vector), with the rotation taken about the
  // vehicle-position-vector (i.e. z-axis).
  // With the biased-target frame, also defined as:
  //  [horizontal in-plane, out-of-plane, vertical-in-plane]
  // the velocity vector can be expressed in biased-target frame as
  // v-hat = [cos(gamma)*cos(H), cos(gamma)*sin(H), sin(gamma)]
  // Hence, v-hat in pfix frame is as follows:
  //-----------------------------------------------------
  double uhat_V[3]; // Unit velocity vector
  for ( unsigned int ii = 0 ; ii < 3 ; ii++ ) {
    uhat_V[ii] =   Position_BiasedTarget_frame.p_hat[ii]*cos_gamma*cos_H
                 + Position_BiasedTarget_frame.n_hat[ii]*cos_gamma*sin_H
                 + Position_BiasedTarget_frame.u_hat[ii]*sin_gamma;
  }

  // check
  TR_Parameter_RefFrame Position_Target_frame("Position-Target");
  Position_Target_frame.compute_frame( Position_BiasedTarget_frame.u_hat,
                                       Target_Reference_frame.u_hat);

  double p_hat_component_of_V = jeod::Vector3::dot( uhat_V,
                                              Position_Target_frame.p_hat);
  if (std::abs(p_hat_component_of_V) < 1.0E-6) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Ambiguous result\n",
      "The generation of the velocity vector from TR-parameters produces 2 "
      "results --\nreflections in the u-n plane of the Position-Target frame.\n"
      "By examining the p- component of velocity, it is usually possible to\n"
      "resolve this ambiguity, but in this case, the p-hat value is too small."
      "\nThe resulting velocity vector may have the wrong sign parallel to the "
      "p-hat vector.\n");
  }
  else {
    target_is_behind_vehicle_check = (p_hat_component_of_V < 0);
  }


  if (target_is_behind_vehicle != target_is_behind_vehicle_check) {
    for ( unsigned int ii = 0 ; ii < 3 ; ii++ ) {
      // correct uhat_V by replacing the contribution to the vector
      // from the term involving cos(H), with the same for -cos(H) --
      // effectively subtract the original and add the negative of it, or
      // substract off double the original.
      uhat_V[ii] -=
              2 * Position_BiasedTarget_frame.p_hat[ii]*cos_gamma*cos_H;
    }
    // The sign of cos_H is now reversed, but isn't used any more so ignore it.
  }


  //----------------------------------------------------
  // Compute velocity vector from speed (Vmag) passed in
  //----------------------------------------------------
  jeod::Vector3::scale( uhat_V,
                  Vmag,
                  velocity );
}




/*****************************************************************************
compute_params_from_vectors
Purpose:
  (Compute the set of target relative parameters from the position vector
  w.r.t the planet center and expressed in the Planet-Center Planet-Fixed
  coordinate system and  the velocity vector w.r.t. the inertial frame
  and expressed in the Planet-Centered Planet-Fixed coordinate system.
  This is used for dispersing the current state:
  - the state is passed into this method to generate a parameter set,
  - the correlated dispersions are applied to this parameter set
  - the modified parameter set is used to generate the modified state via
    compute_*_from_parameters())
*****************************************************************************/
bool
TargetRelative_StateParam::compute_params_from_vectors()
{
  //-----------------------------------------------------
  // Make sure model has been initialized
  //-----------------------------------------------------
  if (!flag_initialized) {
    CMLMessage::error( __FILE__,__LINE__,
      "Attempt to compute the parameters from assigned position/velocity\n"
      "before the model has been initialized.\n"
      "Initialization is required to set the u-hat, N-hat and P-hat\n"
      "vectors relative to the target-reference plane.\n");
    return false;
  }

  // generate the unit vector to the initial position.
  double r_mag = jeod::Vector3::vmag( position );
  if (MathUtils::is_near_equal( r_mag, 0)) {
    CMLMessage::error( __FILE__,__LINE__,
      "The input position has zero magnitude.\n"
      "This makes the parameters also undefined."
      "Setting parameters to zero.\n");
    zero_params();
    return false;
  }
  jeod::Vector3::scale( position,
                  (1/r_mag),
                  uhat_R);

  //-----------------------------------------------------
  // Compute parameters
  // Omit the altitude from the baseline execution,
  //   it forks based on implementation.
  //-----------------------------------------------------

  /* Range angle between initial and target positions*/
  theta_Rng = MathUtils::acos_protected(
                           jeod::Vector3::dot( uhat_R,
                                         Target_Reference_frame.u_hat));

  /* Position rotation angle about target vector */
  theta_Rot = std::atan2( jeod::Vector3::dot( uhat_R,
                                        Target_Reference_frame.n_hat),
                          jeod::Vector3::dot( uhat_R,
                                        Target_Reference_frame.p_hat) );

  /* Position cross angle */
  phi_Cross = MathUtils::asin_protected(
                                jeod::Vector3::dot( uhat_R,
                                              Target_Reference_frame.n_hat));

  /* Inertial velocity magnitude */
  Vmag = jeod::Vector3::vmag( velocity );

  if (MathUtils::is_near_equal( Vmag, 0)) {
    CMLMessage::error( __FILE__,__LINE__,
      "The input velocity has zero magnitude, which leaves the heading plane "
      "undefined.\n"
      "This makes the parameters gamma and lambda also undefined."
     "Setting these parameters to 0.0.\n");
    gamma = 0.0;
    Lambda = 0.0;
    return true;
  }

  /* Flight path angle */
  double      uhat_V[3];      /* (--)  Unit velocity vector       */
  jeod::Vector3::scale( velocity,
                  (1/Vmag),
                  uhat_V);
  gamma = MathUtils::asin_protected( jeod::Vector3::dot( uhat_V,
                                                   uhat_R));
  // Inertial lateral angle consumes the rest of this method.  It is more
  // complicated and requires definition of the biased target -- where
  // the target will be when the vehicle gets there, not where it is now.
  // The lateral angle is measured between (the vector to where the target
  // will be) and the projection of the same vector onto the heading plane
  // (i.e. the plane defined by the vehicle-position and vehicle-velocity
  // vectors).
  // Note -- even though all of these vectors are pfix-referenced, this can
  // be handled pseudo-inertially by momentarily "freezing" the pfix frame
  // and rotating the target vector within the current pfix frame.  The
  // biased target is not really defining a new target in the pfix frame,
  // but providing an estimate of how much the target moves through inertial
  // space *with* the pfix frame.
  if (!compute_biased_target()) {
    CMLMessage::warn( __FILE__,__LINE__,
      "Position vector and BiasedTarget position vector are nearly aligned.\n"
      "In this case, lateral angle is undefined.\n"
      "It is appropriate in this case to return lateral angle (Lambda) value "
      "of zero.\n");
    Lambda = 0.0;
    return true;
  }

  // Compute sine of target heading angle, H:
  // Notes:
  // -> H is angle between the the planes defined by the:
  //     vehicle-position vector and biased-target-position vector, and
  //     vehicle_position vector and vehicle-velocity vector.
  // -> component of v-hat along perpendicular to r-targ plane is:
  //     -> v-hat dot Nhat, and
  //     -> cos(gamma) * sin(H) =>
  //        v-hat dot Nhat = cos(gamma) * sin(H)

  // Compute the cosine of flight path angle
  double cos_gamma = std::cos(gamma);

  // Check if uhat_R and uhat_V are aligned.
  // Checking on cos-gamma rather than sin-gamma
  // only because cos(pi/2) = cos (-pi/2)
  if ( std::abs(cos_gamma) < epsilon ) {
    // When position and velocity vectors are nearly aligned,
    // lateral angle is undefined.  It is appropriate in this
    // case to return a lateral angle value of zero.
    CMLMessage::warn(
     __FILE__,__LINE__,"Configuration warning\n",
     "Position and velocity vector inputs are nearly aligned."
     "\nThis is unusual but manageable.  Check configuration.\n");
    Lambda = 0.0;
    return true;
  }

  // Compute sine of target heading angle, H
  double sin_H = MathUtils::divide_protected(
                                 jeod::Vector3::dot(uhat_V,
                                              Position_BiasedTarget_frame.n_hat),
                                 cos_gamma,
                                 0.0,
                                 true);

  // Compute lateral angle, Lambda
  Lambda = MathUtils::asin_protected( sin_H * std::sin( theta_RngBias));
  return true;
}



/*****************************************************************************
compute_position_from_params
Purpose:
 (Compute the position vector w.r.t. the planet center and expressed in
  the Planet-Centered Planet-Fixed coordinate system from the geodetic
  altitude, range angle, and either rotation angle or cross angle.)
*****************************************************************************/
bool
TargetRelative_StateParam::compute_position_from_params(
  InputPosAngle input_theta_type)
{
  //-----------------------------------------------------
  // Make sure model has been initialized
  //-----------------------------------------------------
  if (!flag_initialized) {
    CMLMessage::error( __FILE__,__LINE__,
      "Attempt to compute the position from assigned parameters before the\n"
      "model has been initialized.\n"
      "Initialization is required to set the u-hat, N-hat and P-hat vectors\n"
      "for the plane defined by the target and reference vectors.\n"
      "Aborting calculation of position vector.\n");
    return false;
  }

  // Compute sine and cosine of range angle
  double sin_theta_Rng = std::sin(theta_Rng);
  double cos_theta_Rng = std::cos(theta_Rng);

  /* Compute sine and cosine of rotation angle
   * This is done in one of several ways, so create the variables, then
   * populate them.*/
  //-----------------------------------------------------
  double sin_theta_Rot; /* Sin of rotation angle */
  double cos_theta_Rot; /* Cos of rotation angle */

  // If using theta_Rot (default), just take the sine and cosine directly:
  if (input_theta_type == INPUT_THETA_ROT) {
    sin_theta_Rot = std::sin(theta_Rot);
    cos_theta_Rot = std::cos(theta_Rot);
  }
  /* Otherwise, using cross-angle (phi_Cross) as the input; ignore the
   * rotation-angle values and generate the sin and cosine of
   * rotation-angle from the cross-angle instead.
   * First, make sure range angle is large enough to support a
   * cross-angle. If the range-angle is zero, ignore the cross-angle value
   * and continue with the assumption that the rotation angle is zero.
   * Post a warning about inconsistent data if the cross-angle is non-zero.*/
  else if ( std::abs( sin_theta_Rng) < epsilon ) {
    sin_theta_Rot = 0.0;
    cos_theta_Rot = 1.0;
    if ( std::abs(phi_Cross) > 0 ) {
      CMLMessage::warn( __FILE__,__LINE__,
        "Range angle (theta_Rng) input is near zero.\n"
        "In this case, the cross angle (phi_Cross) input should be zero.\n"
        "Ignoring the non-zero cross angle input (",phi_Cross,").\n"
        "Check configuration.\n");
    }
  }
  /* Otherwise, we are using cross-angle with a non-zero range-angle so the
   * rotation-angle could be non-zero and needs computing. We don't actually
   * need the rotation-angle, only the sine and cosine of it.*/
  else {
    double sin_phi = std::sin(phi_Cross);
    // Compute sin(theta_Rot); theta argument is phi
    // Fail if |sin(phi)| > |sin(range)| (or if both are zero)
    sin_theta_Rot = MathUtils::divide_protected( sin_phi,
                                                 sin_theta_Rng);

    /* Check for valid combination of phi and theta_Rng */
    if ( std::abs( sin_theta_Rot) > 1.0 ) {
      CMLMessage::error( __FILE__,__LINE__,
        "Questionable results for inputs phi and theta_Rng!!\n",
        "  phi        = ",phi_Cross,"\n"
        "  theta_Rng  = ",theta_Rng,"\n"
        "Results in:\n"
        "  sin_theta_Rot = 1 + (",sin_theta_Rot-1,")\n"
        "Adjusting sin_theta_Rot to +/- 1.0\n\n");
      sin_theta_Rot = (sin_theta_Rot>0) ? 1.0 : -1.0;
    }

    // Compute cos(theta_Rot)
    cos_theta_Rot =  std::sqrt(1 - sin_theta_Rot*sin_theta_Rot);
    if (input_theta_type == INPUT_PHI_ABS_THETA_ROT_GT_90) {
      cos_theta_Rot *= -1.0;
    }
  }


  //-----------------------------
  // Compute unit position vector
  // r-hat =  [sin(theta_rng), 0, cos(theta_rng)] in a frame where axes are:
  //  defined by the target plane (the plane defined by the
  //  target-position vector and vehicle-position vector)
  // that plane is a rotation by angle theta_rot relative to the
  // reference-target plane (the plane defined by the target-position vector and
  // the reference-position vector), with the rotation taken about the
  // target-position-vector (i.e. z-axis).
  // The position vector can be expressed in the reference-target frame as
  //
  //         [cos(theta_rot)*sin(theta_rng)]
  // r-hat = [sin(theta_rot)*sin(theta_rng)]
  //         [cos(theta_rng)               ]
  //
  // Then the axes of this frame can be used to express this in pfix:
  //-----------------------------
  for ( unsigned int ii = 0 ; ii < 3 ; ii++ ) {
    uhat_R[ii] =   Target_Reference_frame.p_hat[ii]*sin_theta_Rng*cos_theta_Rot
                 + Target_Reference_frame.n_hat[ii]*sin_theta_Rng*sin_theta_Rot
                 + Target_Reference_frame.u_hat[ii]*cos_theta_Rng;
  }
  return true;
}



/*****************************************************************************
compute_biased_target
Purpose:
  Bias the target vector by rotating it about the planetary rotation axis
  using an estimate of the rotation that will occur during flight.  This is
  done by computing an estimated time of flight and rotating the target
  vector about the planetary axis of rotation.
Note: Iterating once (two calculations) on theta_RngBias  is equivalent to the
    implementation of the Apollo Final Phase algorithm in Predguid during
    initialization
*****************************************************************************/
bool
TargetRelative_StateParam::compute_biased_target()
{
  // Initial guess for theta_RngBias is equal to params.thetaRng ... except
  // that params.thetaRng may not have been populated in the case that the
  // velocity is being computed from parameters.
  // So compute it, just in case.
  // Take the dot product of the unit vectors for the initial and target
  // positions and the arc-cosine of the result:
  theta_RngBias = MathUtils::acos_protected(
                         jeod::Vector3::dot( uhat_R,
                                       Target_Reference_frame.u_hat));

  // If omega_mag is zero or target lies on the rotation axis, this is the
  // correct value for theta_RngBias because the target is unmoved by
  // rotation, so the biased_target = target.
  // In this case, simply use the target vector to construct the
  // Position-BiasedTarget frame
  if ( MathUtils::is_near_equal( omega_mag, 0) ||
       omega_target_aligned) {
    jeod::Vector3::copy( Target_Reference_frame.u_hat,
                   biased_target_pos_hat);
  }
  else {
    // Continue to the main algorithm, which involves rotating the target
    // vector about the omega axis.
    if (MathUtils::is_near_equal( K_theta, 0) &&
        !k_theta_warning_sent) {
      CMLMessage::warn( __FILE__,__LINE__,
       "The value of K_theta is close to zero, which probably means it has "
       "not been set.\n"
       "This will result in invalid biasing of the target.\n"
       "Check configuration for setting of K_theta; computation will "
       "proceed.\n");
       k_theta_warning_sent = true;
    }


    // Start the iternation:
    for ( unsigned int ii = 0 ; ii < 2 ; ii++ ) {
      // Compute the rotation angle of the biased target about the planet
      // rotation axis (theta_omega)
      double theta_omega = omega_mag * K_theta * theta_RngBias;

      /* Compute sine and cosine of theta_omega */
      double sin_theta_omega = std::sin(theta_omega);
      double cos_theta_omega = std::cos(theta_omega);


      // Compute rotated target vector
      // Consider the Omega-Target frame
      // With:
      //  - u_hat aligned wih omega and
      //  - the target vector lying in the u-hat, p-hat plane, and
      // - phi defined as the angle between the target vector and omega, and
      // - theta being the angle by which the vector is rotated,
      // we have the target unit vector described as ([u,n,p]):
      //     [cos(phi), 0, sin(phi)]
      // and the biased-target unit-vector is:
      // [cos(phi), sin(theta) sin_(phi), cos(theta) sin(phi)]

      for( unsigned int jj = 0 ; jj < 3 ; jj++ ) {
        biased_target_pos_hat[jj] =
            Omega_Target_frame.u_hat[jj]*cos_phi_omega
          + Omega_Target_frame.n_hat[jj]*sin_phi_omega*sin_theta_omega
          + Omega_Target_frame.p_hat[jj]*sin_phi_omega*cos_theta_omega;
      }

      // update the angle between position vector and biased target vector
      // (theta_RngBias) for next iteration
      double dot_prod = jeod::Vector3::dot( uhat_R,// Unit position
                                      biased_target_pos_hat);
      theta_RngBias = MathUtils::acos_protected( dot_prod);
    }
  }

  if (!Position_BiasedTarget_frame.compute_frame( uhat_R,
                                                  biased_target_pos_hat)) {
    /* Position vector and BiasedTarget position vector are nearly aligned.
     * Consequences depend on where this was called from.
     * return false and let the source sort it out. Both cases produce a
     * warning message.*/
    return false;
  }
  return true;
}
