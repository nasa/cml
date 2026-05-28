/*******************************TRICK HEADER******************************
PURPOSE: (Disperses the state)

PROGRAMMERS:
  (((Gary Turner) (OSR) (February 2015) (Antares) (Initial version))
   ((Brenton Caughron) (OSR) (Oct. 2017) (Antares) (IV&V Code Review))
   ((Daniel Ghan) (OSR) (October 2019) (Antares) (Use C++ rand instead of
               Trick rand_num, added option for user-input dispersions)))
************************************************************************/

#include <cstring>  // memset
#include <cmath>    // sqrt
#include "jeod/models/utils/math/include/vector3.hh"    // jeod::Vector3::*
#include "jeod/models/utils/math/include/matrix3x3.hh"  // jeod::Matrix3x3::*

#include "../include/correlated_state_dispersion.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
CorrelatedStateDispersion::CorrelatedStateDispersion()
  :
  pv_covar_frame(PV_COVAR_INRTL),
  corr_option(CORRELATED_PV),
  corr_att_frame(REFERENCE_FRM),
  att_rot_defined(PerturbedToTrue),
  dispersion_distribution(NoDispersion),
  hold_previous_random_vec(false),
  max_iterations(2000),
  sigma_limit(0.0),
  seed(12345),
  user_specified_distribution{0.0},
  covariance{0.0},
  corr_base_frame_name(""),
  TR_geodetic_altitude_disp(0.0),
  pos_error{0.0, 0.0, 0.0},
  vel_error{0.0, 0.0, 0.0},
  att_error{0.0, 0.0, 0.0},
  prm_error{0.0, 0.0, 0.0, 0.0, 0.0},
  r_mag(0.0),
  declination(0.0),
  right_asc(0.0),
  v_mag(0.0),
  fp_angle(0.0),
  azimuth(0.0),
  generator_seeded(false),
  dimension(6),
  random_vec{0.0},
  sqrt_covariance{0.0},
  rand_uniform(0.0, 1.0),
  rand_norm(0.0, 1.0)
{}

/********************************************************************************
disperse_state (5 arguments)
Purpose:(
  - Transform the position, velocity from trans_base to cov_base.
  - Transform the attitude from cov_base to rot_init's target frame.
  - Generate dispersed state (P, V, and A).
  - Transform the dispersed position and velocity from cov_base to trans_base.
  - Transform the dispersed attitude from trans_base to rot_init's target frame.)
Note:    NULL checks performed in 3-argument disperse_state.
********************************************************************************/
void
CorrelatedStateDispersion::disperse_state(
     const jeod::RefFrameState & trans_base_wrt_cov_base,
     const double (&rot_base_wrt_cov_base)[3][3],
     double position[3],          // wrt trans_base, expressed in trans_base
     double velocity[3],          // wrt trans_base, expressed in trans_base
     double (&trans_mx)[3][3])    // rot_base to rot_init's target frame
{
  double pos_wrt_cov_base[3];
  double vel_wrt_cov_base[3];

  // Transform the position wrt a trans_base, expressed in a trans_base
  // to a cov_base, expressed in a cov_base
  jeod::Vector3::transform_transpose(trans_base_wrt_cov_base.rot.T_parent_this,
                               position,
                               pos_wrt_cov_base);
  jeod::Vector3::incr(trans_base_wrt_cov_base.trans.position,
                pos_wrt_cov_base);
 
  // Transform the velocity wrt a trans_base, expressed in a trans_base
  // to a cov_base, expressed in a cov_base
  jeod::Vector3::transform_transpose(trans_base_wrt_cov_base.rot.T_parent_this,
                               velocity,
                               vel_wrt_cov_base);
  jeod::Vector3::incr(trans_base_wrt_cov_base.trans.velocity,
                vel_wrt_cov_base);
 
  // Compute the attitude of rot_init's target frame relative to the cov_based frame
  // i.e. the transformation matrix from cov_base to rot_init's target frame
  double T_cov_base_to_this[3][3];
  jeod::Matrix3x3::product(trans_mx,              // rot_base to rot_init's target frame
                     rot_base_wrt_cov_base, // cov_base to rot_base
                     T_cov_base_to_this);   // cov_base to rot_init's target frame

  disperse_state(pos_wrt_cov_base, vel_wrt_cov_base, T_cov_base_to_this);
  
  // Transform the position wrt a cov_base, expressed in a cov_base
  // to a trans_base, expressed in a trans_base
  jeod::Vector3::decr(trans_base_wrt_cov_base.trans.position,
                pos_wrt_cov_base);
  jeod::Vector3::transform(trans_base_wrt_cov_base.rot.T_parent_this,
                     pos_wrt_cov_base,
                     position);

  // Transform the velocity wrt a cov_base, expressed in a cvr_base
  // to a trans_base, expressed in a trans_base
  jeod::Vector3::decr(trans_base_wrt_cov_base.trans.velocity,
                vel_wrt_cov_base);
  jeod::Vector3::transform(trans_base_wrt_cov_base.rot.T_parent_this,
                     vel_wrt_cov_base,
                     velocity);

  // Compute the attitude of rot_init's target frame relative to the rot_based frame
  // i.e. the transformation matrix from rot_base to rot_init's target frame
  jeod::Matrix3x3::product_right_transpose(T_cov_base_to_this,    // cov_base to rot_init's target frame
                                     rot_base_wrt_cov_base, // cov_base to rot_base
                                     trans_mx);             // rot_base to rot_init's target frame
}

/*****************************************************************************
disperse_state (4 arguments)
Purpose:(
  - Tramsform the position, velocity and attitude from state_base to cov_base.
  - Gernerate dispersed state (P, V, and A).
  - Trasform the dispersed position, velocity and attitude from cov_base to
    state_base.)
Note:    NULL checks performed in 5-argument disperse_state.
*****************************************************************************/
void
CorrelatedStateDispersion::disperse_state(
     const jeod::RefFrameState & state_base_wrt_cov_base,
     double position[3],
     double velocity[3],
     double (&trans_mx)[3][3])
{
  disperse_state(state_base_wrt_cov_base,
                 state_base_wrt_cov_base.rot.T_parent_this,
                 position,
                 velocity,
                 trans_mx);
}
/*****************************************************************************
disperse_state (3 arguments)
Purpose:(
  - Calls the 2-argument version of disperse_state, passing the position
    and velocity vectors through.
  - If full correlation desired (P, V, and A), the 2-argument version of
    disperse_state will generate the A dispersions, then
    apply_attitude_dispersions will apply them.)
Note:    NULL checks performed in 2-argument disperse_state.
*****************************************************************************/
void
CorrelatedStateDispersion::disperse_state(
     double position[3],
     double velocity[3],
     double (&trans_mx)[3][3])
{
  if (disperse_state(position, velocity) && corr_option == CORRELATED_PV_ATT) {
    apply_attitude_dispersions(trans_mx);
  }
}
/*****************************************************************************
disperse_state (2 arguments)
Purpose:(
 -  Generates the state dispersions.
   -  If corr_option == CORRELATED_PV, generates P and V dispersions
   -  If corr_option == CORRELATED_PV_ATT, generates P, V, and A dispersions
 -  Overwrites the position and velocity values using the new dispersed
    position and velocity values.
   -  If corr_option == CORRELATED_PV_ATT, the dispersions on the
      transformation-matrix values will be stored and can be applied later
      to overwrite the transformation matrix values using the
      apply_attitude_dispersions method.)

Note:    NULL checks performed in apply_translational_dispersions.
*****************************************************************************/
bool
CorrelatedStateDispersion::disperse_state(
     double position[3],
     double velocity[3])
{
  if (!initialize()) { return false; }

  // Identify how many elements we need
  switch (corr_option)
  {
   case CORRELATED_PV: // constructor-default
    // Identify whether we need 5 or 6 elements
    //  5 if using parameter-set
    //  6 if using position-velocity
    dimension = (pv_covar_frame == PV_COVAR_TR_PARAM) ? 5 : 6;
    break;
   case CORRELATED_PV_ATT:
    dimension = 9;
    break;
   default:
    CMLMessage::fail(
      __FILE__, __LINE__, "Invalid corr_option\n",
      "corr_option must be valid when calling disperse_state with\n"
      "multiple arguments.\n");
  }

  generate_dispersions();
  apply_translational_dispersions(position, velocity);
  return true;
}
/*****************************************************************************
disperse_state (1 argument)
Purpose:
 -  Generates the full state dispersions (P, V, and A dispersions)
 -  Overwrites the Transformation-matrix values using the new dispersed
    values.
 -  The dispersions on the position and velocity will be stored and can be
    applied later using the apply_translational_dispersions method.
*****************************************************************************/
void
CorrelatedStateDispersion::disperse_state(
     double (&trans_mx)[3][3])
{
  if (!initialize()) { return; }

  dimension = 9;
  generate_dispersions();
  apply_attitude_dispersions(trans_mx);
}

/*****************************************************************************
apply_translational_dispersions
Purpose:(Transforms the generated translational dispersions from the frame in
           which they were specified into the inertial frame and applies them
           to position and velocity.)
*****************************************************************************/
void
CorrelatedStateDispersion::apply_translational_dispersions(
     double position[3],
     double velocity[3])
{
  if (position == NULL || velocity == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid input\n.",
      "One or both of the position and velocity vectors passed in are NULL.\n"
      "Aborting state dispersion.\n");
    return;
  }

  switch(pv_covar_frame) {
   case PV_COVAR_INRTL:
   case PV_COVAR_REL_LVLH:
    /* Errors are Inertial or relative LVLH, so no conversion needed */
    break;
   case PV_COVAR_LVLH:
    /* Errors are LVLH, so transform to Inertial */
    double T_inrtl_lvlh[3][3];
    MathUtils::generate_inertial_to_lvlh( position,
                                          velocity,
                                          T_inrtl_lvlh);
    jeod::Vector3::transform_transpose(T_inrtl_lvlh, pos_error);
    jeod::Vector3::transform_transpose(T_inrtl_lvlh, vel_error);
    break;
   case PV_COVAR_UVW:
    double T_inrtl_uvw[3][3];
    /* Errors are UVW, so transform to Inertial */
    MathUtils::generate_inertial_to_uvw( position,
                                         velocity,
                                         T_inrtl_uvw);
    jeod::Vector3::transform_transpose(T_inrtl_uvw, pos_error);
    jeod::Vector3::transform_transpose(T_inrtl_uvw, vel_error);
    break;

   case PV_COVAR_R_DEC_RA_V_FP_AZ:
    transform_ra_dec_fp(position, velocity);
    break;

   case PV_COVAR_TR_PARAM:
    transform_TR_param(position, velocity);
    break;

   default:
    CMLMessage::fail(
      __FILE__, __LINE__, "Invalid covariance frame\n",
      "Covariance frame option not found.\n");
  }

  // Add the now-inertial error values to position and velocity.
  jeod::Vector3::incr(pos_error, position);
  jeod::Vector3::incr(vel_error, velocity);
}

/*****************************************************************************
apply_attitude_dispersions
Purpose:(Applies the attitude dispersions to the transformation matrix)
*****************************************************************************/
void
CorrelatedStateDispersion::apply_attitude_dispersions(
     double (&trans_mx)[3][3])
{
  // Transformation matrix representative of the attitude dispersions
  //     interpreted as Euler angles:
  // att_error are small angles about x,y,z so RPY sequence is used
  //     to preserve the correct order
  double euler_transform[3][3];
  jeod::Orientation::compute_matrix_from_euler_angles(
                   jeod::Orientation::Roll_Pitch_Yaw,
                   att_error,
                   euler_transform);

  // Copy of the trans_mx transformation matrix.  Necessary because the
  // Matrix3x3 class operations cannot self-operate like the
  // Vector3 operators can.
  double copy_of_trans_mx[3][3];
  jeod::Matrix3x3::copy(trans_mx, copy_of_trans_mx);

  if (corr_att_frame == REFERENCE_FRM) { // apply error to reference frame
    // if nav-init is perturbed-to-true or state-init is true-to-nominal:
    if (att_rot_defined == PerturbedToTrue) {
      // "Add" attitude dispersion
      jeod::Matrix3x3::product(copy_of_trans_mx,
                         euler_transform,
                         trans_mx);
    }
    else {
       // Transform ref-frame dispersion before applying
       jeod::Matrix3x3::product_right_transpose(copy_of_trans_mx,
                          euler_transform,
                          trans_mx);
    }
  }
  else { //BODY_FRM: apply error to body-frame
    if (att_rot_defined == PerturbedToTrue) {
      // Transform body-frame dispersion before applying
      jeod::Matrix3x3::product_left_transpose(euler_transform,
                                        copy_of_trans_mx,
                                        trans_mx);
    }
    else {
      // "Add" attitude dispersion
      jeod::Matrix3x3::product(euler_transform,
                         copy_of_trans_mx,
                         trans_mx);
    }
  }
}

/*****************************************************************************
initialize
Purpose:(init checks)
*****************************************************************************/
bool
CorrelatedStateDispersion::initialize()
{
  // If model is configured to apply no dispersion, there is nothing to do!
  if (dispersion_distribution == NoDispersion) {
    return false;
  }
  // Initialize the random-number generator with the specified seed and return
  // true to indicate the model should proceed with dispersions.
  if (!generator_seeded) {
    generator.seed(seed);
    generator_seeded = true;
  }
  return true;
}

/*****************************************************************************
generate_dispersions
Purpose:(Generates the dispersions that will be added to each element of the
         state.)
*****************************************************************************/
void
CorrelatedStateDispersion::generate_dispersions()
{
  // If relevant, check for a valid dispersion limit.
  if (dispersion_distribution == Uniform ||
      dispersion_distribution == TruncatedGaussian) // or TruncatedNormal
  {
    if (MathUtils::is_near_equal(sigma_limit, 0.0))
    {
      CMLMessage::warn (
        __FILE__, __LINE__, "Invalid limit\n",
        "Tried to use a bounded distribution with the value sigma_limit still\n"
        "at its default 0.0. This would result in no dispersion.\n"
        "Resetting value to 3.0 to provide 3-sigma limit.\n");
      sigma_limit = 3.0;
    }

    if (sigma_limit < 0.0)
    {
      CMLMessage::warn(
        __FILE__, __LINE__, "Negative limit\n",
        "Negative number specified for sigma_limit. Using its absolute "
        "value.\n");
      sigma_limit = -sigma_limit;
    }
  }

  // Random vector generation
  if (!hold_previous_random_vec) {
    generate_random_vec();
  }

  // Compute the "square root" of the covariance matrix
  if (!MathUtils::cholesky_decomposition( "CorrelatedStateDispersion",
                                          &covariance[0][0],
                                          &sqrt_covariance[0][0],
                                          9,
                                          dimension)) {
    CMLMessage::fail(
      __FILE__,__LINE__,"Decomposition failed\n",
      "Cholesky decomposition of the covariance matrix failed.\n"
      "Check that the covariance is valid and has been entered correctly.\n");
  }

  // Compute dispersions (sqrt_covariance * random_vec). By the nature of the
  //     Cholesky decomposition, sqrt_covariance is a lower triangular matrix,
  //     so the elements above the diagonal do not have to be included in the
  //     multiplication.
  if (dimension == 5) { // i.e. using parameter-set to disperse position-velocity
    for( unsigned int ii = 0; ii < 5; ii++) {
      prm_error[ii] = sqrt_covariance[ii][0] * random_vec[0];
      for (unsigned int jj = 1; jj <= ii; jj++) {
        prm_error[ii] += sqrt_covariance[ii][jj] * random_vec[jj];
      }
    }
  }
  else {
    for( unsigned int ii = 0; ii < 3; ii++){
      pos_error[ii] = sqrt_covariance[ii][0] * random_vec[0];
      for (unsigned int jj = 1; jj <= ii; jj++) {
        pos_error[ii] += sqrt_covariance[ii][jj] * random_vec[jj];
      }

      vel_error[ii] = sqrt_covariance[ii+3][0] * random_vec[0];
      for (unsigned int jj = 1; jj <= ii+3; jj++) {
        vel_error[ii] += sqrt_covariance[ii+3][jj] * random_vec[jj];
      }

      if (dimension == 9) {
        att_error[ii] = sqrt_covariance[ii+6][0] * random_vec[0];
        for (unsigned int jj = 1; jj <= ii+6; jj++) {
          att_error[ii] += sqrt_covariance[ii+6][jj] * random_vec[jj];
        }
      }
    }
  }
}

/*****************************************************************************
generate_random_vec
Purpose: Generate a new set of random values.  The vector of random values
         will be sized according to how many variables are being dispersed;
         each variable will be given a dispersion equal to the product of its
         random number (generated here) and its standard-deviation (generated
         by taking the "square-root" of the the covariance matrix
*****************************************************************************/
void
CorrelatedStateDispersion::generate_random_vec()
{
  switch (dispersion_distribution) {
  case Uniform:
    populate_random_vec_uniform_sphere();
    break;

  case Gaussian:
    populate_random_vec_norm();
    break;

  case TruncatedGaussian:
  {
    // Generate the direction of random_vec, same as for Uniform
    populate_random_vec_unit();
    // Generate the length of random_vec. The square of the radius follows a
    // chi-squared distribution with <dimension> degrees of freedom.
    double radius_sq = 0.0;
    double sigma_lim_sq = sigma_limit * sigma_limit;
    std::chi_squared_distribution<double> rand_chi2(dimension);
    unsigned int num_iterations = 0;
    do
    {
      // generate a chi-sq-distributed value for the square of the radius.
      radius_sq = rand_chi2(generator);
    }
    while (++num_iterations < max_iterations && radius_sq > sigma_lim_sq);
    if (radius_sq > sigma_lim_sq) {
      CMLMessage::error(
        __FILE__, __LINE__, "Limit too small for TruncatedGaussian\n",
        "Reached the maximum number of iterations (", max_iterations, ") trying to randomize\n"
        "the dispersions using a truncated Gaussian distribution. Will use\n"
        "a uniform distribution (with the same boundary) instead. To use a\n"
        "Gaussian distribution, increase sigma_limit or max_iterations.\n");
      populate_random_vec_uniform_sphere();
      break;
    }

    double radius = std::sqrt(radius_sq);
    for (unsigned int ii = 0; ii < dimension; ii++) {
      random_vec[ii] *= radius;
    }
    break;
  }

  case Constant:
    for (unsigned int ii = 0; ii < dimension; ii++) {
      random_vec[ii] = sigma_limit;
    }
    break;

  case UserInput:
    for (unsigned int ii = 0; ii < dimension; ii++) {
      random_vec[ii] = user_specified_distribution[ii];
    }
    break;

  default:
    CMLMessage::fail(
      __FILE__, __LINE__, "Invalid Dispersion Distribution\n",
      "Invalid option specified for dispersion_distribution!\n");
  }
}

/*****************************************************************************
populate_random_vec_norm
Purpose:(Populates random_vec with normally-distributed random variables)
*****************************************************************************/
void
CorrelatedStateDispersion::populate_random_vec_norm()
{
  for (unsigned int ii = 0; ii < dimension; ii++) {
    random_vec[ii] = rand_norm(generator); }
}

/*****************************************************************************
populate_random_vec_unit
Purpose:(Populates random_vec with a random unit vector)
*****************************************************************************/
void
CorrelatedStateDispersion::populate_random_vec_unit()
{
  populate_random_vec_norm();
  double radius_sq = 0.0;
  for (unsigned int ii = 0; ii < dimension; ii++) {
    radius_sq += random_vec[ii]*random_vec[ii];
  }
  double radius = std::sqrt(radius_sq);

  if (MathUtils::is_near_equal(radius, 0.0)) {
    CMLMessage::error(__FILE__, __LINE__, "Random number generation\n",
      "The random number generator returned all zeros. The probability of\n"
      "this happening by chance is infinitesimally small, so something is\n"
      "almost certainly wrong.\nCannot generate random unit vector. State "
      "will not be dispersed.");
  }
  else {
    for (unsigned int ii = 0; ii < dimension; ii++) {
      random_vec[ii] /= radius;
    }
  }
}

/*****************************************************************************
populate_random_vec_uniform_sphere
Purpose:(Populates random_vec uniformly within a sphere with radius sigma_limit)
*****************************************************************************/
void
CorrelatedStateDispersion::populate_random_vec_uniform_sphere()
{
  // Generate an array of random values, one per dispersed variable
  // Normalize the array such that it has a radius of 1.  This produces a
  // "unit vector" to the dispersed value through the n-dimensional space
  // where n is the number of correlated variables.
  populate_random_vec_unit();
  // Now generate the length of the vector, oriented along this unit-vector
  // Start with a length between 0 and 1, take the nth root of this value to
  // make the distribution uniform across the hyper-volume, then scale by the
  // desired sigma-limit (see documentation for more comprehensive
  // description)
  double radius = rand_uniform(generator);
  radius = sigma_limit * std::pow(radius, 1.0 / dimension);
  for (unsigned int ii = 0; ii < dimension; ii++) {
    random_vec[ii] *= radius; }
}

/*****************************************************************************
transform_ra_dec_fp
Purpose:(long branch from the switch statement for case
         PV_COVAR_R_DEC_RA_V_FP_AZ.)
*****************************************************************************/
void
CorrelatedStateDispersion::transform_ra_dec_fp(
     const double position[3],
     const double velocity[3])
{
  // Note - protected method; position and velocity already checked for not-NULL

  // Errors are: r_mag, declination,       right ascension,
  //             v_mag, flight-path-angle, azimuth

  double T_inrtl_to_NED[3][3]; // inertial to NED (North-East-Down)
  // Compute reference position params from input Inertial state
  r_mag       = jeod::Vector3::vmag( position);

  // safety check:
  if (MathUtils::is_near_equal(r_mag, 0.0)) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid position vector\n",
      "The position vector has zero magnitude.\n"
      "Assigning RA and dec to 0.0 and aligning NED with inertial\n");
    declination = 0.0;
    right_asc = 0.0;
    jeod::Matrix3x3::identity(T_inrtl_to_NED);
  }
  else {
    declination = std::asin(position[2] / r_mag);
    right_asc   = std::atan2(position[1], position[0]);

    //  Generate the frame-transformation between the inertial-frame and the NED
    //  frame. Note that this is only generating the transformation matrix, not
    //  the actual NED frame.  As a NED-frame definition, it is incomplete
    //  (or atypical) because it does not account for planetary rotation.
    //  However, we are only using it for transformations, not for state
    //  representations, so this is adequate.
    double r_unit[3];
    jeod::Vector3::scale( position, (1/r_mag), r_unit);
    double pos_xy_mag    =  std::sqrt( r_unit[0] * r_unit[0] + r_unit[1] * r_unit[1]);
    double unit0_xy_div  =  r_unit[0] /pos_xy_mag;
    double unit1_xy_div  = -r_unit[1] / pos_xy_mag;

    T_inrtl_to_NED[0][0] = -unit0_xy_div * r_unit[2];
    T_inrtl_to_NED[0][1] =  unit1_xy_div * r_unit[2];
    T_inrtl_to_NED[0][2] =  pos_xy_mag;
    T_inrtl_to_NED[1][0] =  unit1_xy_div;
    T_inrtl_to_NED[1][1] =  unit0_xy_div;
    T_inrtl_to_NED[1][2] =  0;
    T_inrtl_to_NED[2][0] = -r_unit[0];
    T_inrtl_to_NED[2][1] = -r_unit[1];
    T_inrtl_to_NED[2][2] = -r_unit[2];
  }

  // Compute reference vel params from input Inertial state
  v_mag = jeod::Vector3::vmag( velocity);

  // Compute flight-path-angle and azimuth
  double velocity_NED[3];
  jeod::Vector3::transform(T_inrtl_to_NED, velocity, velocity_NED);
  double velocity_mag_horiz_plane = std::sqrt(velocity_NED[0]*velocity_NED[0] +
                                              velocity_NED[1]*velocity_NED[1]);

  fp_angle = std::atan2(-velocity_NED[2], velocity_mag_horiz_plane);
  azimuth  = std::atan2( velocity_NED[1], velocity_NED[0]);

  // Add errors to reference parameters
  // (pos_error) and (vel_error) variables do not have units, for this use case
  // with the RA, dec, fp, and AZ the error values going into them need to be
  // of angle unit values. This is because a normal
  // position dispersion of .7 meters or so is a much bigger dispersion in
  // rotational units when considering the angles are represented on the Earth.
  //
  // To ensure the values are correct going in, the covariance matrix inputted
  // into the model needs to have certain values in correct order of magnitude
  // so that the error values aren't too large when the RA, dec, fp, and AZ are
  // all incremented.
  //
  // Each error value pertains to a specific row of the sqrt_covariance matrix
  // which correlates to a specific row in the regular covariance matrix.
  //
  // pos_error[0] = sum(row 1)*rndm_value
  // pos_error[1] = sum(row 2)*rndm_value
  // pos_error[2] = sum(row 3)*rndm_value
  // vel_error[0] = sum(row 4)*rndm_value
  // vel_error[1] = sum(row 5)*rndm_value
  // vel_error[2] = sum(row 6)*rndm_value
  //
  // Where each row specified is the the row of the bottom left half of the
  // sqrt_covariance matrix, going from each rows starting [0] value up to the
  // diagonal value for that row. i.e. [ii][0] --> [ii][ii]
  //
  // So the values in rows 2,3,5 and 6 need to be of an angular magnitude so as
  // to correctly match the unit type of the variables the values are
  // incremented into.
  r_mag       += pos_error[0];
  declination += pos_error[1];
  right_asc   += pos_error[2];

  v_mag       += vel_error[0];
  fp_angle    += vel_error[1];
  azimuth     += vel_error[2];

  // Compute errored Inertial state and output errors
  // xy here refers to inertial {x,y} not to NED {x,y}
  double r_xy_mag = r_mag     * std::cos(declination);
  pos_error[0]    = r_xy_mag  * std::cos(right_asc)   - position[0];
  pos_error[1]    = r_xy_mag  * std::sin(right_asc)   - position[1];
  pos_error[2]    = r_mag     * std::sin(declination) - position[2];

  velocity_mag_horiz_plane =  v_mag                    * std::cos(fp_angle);
  velocity_NED[0]          =  velocity_mag_horiz_plane * std::cos(azimuth);
  velocity_NED[1]          =  velocity_mag_horiz_plane * std::sin(azimuth);
  velocity_NED[2]          = -v_mag                    * std::sin(fp_angle);

  // Transform to inertial and subtract off the original velocity to find
  // the error.
  // vel_error_inrtl = T_NED_to_inrtl*new_vel_NED - original_velocity
  jeod::Vector3::transform_transpose( T_inrtl_to_NED, velocity_NED, vel_error);
  jeod::Vector3::decr( velocity, vel_error);
}

/*****************************************************************************
transform_TR_param
Purpose: (This method is used if the covariance matrix is defined as type
         PV_COVAR_TR_PARAM.  It generates the state error, expressed
         as a {Position,velocity} pair when the covariance matrix has no direct
         interpretation of position and velocity components.
       Starting with the input state as described by a {Position, velocity}
       (PV)-set, generate a
       Target-Referenced Parameter-set that represents the same state.
       The covariance matrix defines the errors on these
       variables as a function of the values themselves.  Thus, errored-values
       can be obtained for the Target-Referenced Parameter-set, and these can
       be converted back into an errored PV-set.
       Next, by differencing the errored PV-set and the original PV-set, the
       error can be described as a PV-set.)
*****************************************************************************/
void
CorrelatedStateDispersion::transform_TR_param(
     const double position[3],
     const double velocity[3])
{
  /* TR_param.pfix-position.planet must have been configured prior to use */
  if (TR_param.pfix_position.planet == NULL) {
    CMLMessage::fail(
    __FILE__,__LINE__,"Invalid planet specification\n",
    "Planet has not been assigned to the PlanetFixedPosition instance\n"
    "(TR_param.pfix_position). "
    "This instance is necessary for correct operation.\n");
  }

  /* Transform position and velocity into Planet-Centered Planet-Fixed */
  double R_PCPF[3];
  jeod::Vector3::transform( TR_param.pfix_position.planet->pfix.state.rot.T_parent_this,
                      position,
                      R_PCPF);
  double V_wrt_PCI_PCPF[3];
  jeod::Vector3::transform( TR_param.pfix_position.planet->pfix.state.rot.T_parent_this,
                      velocity,
                      V_wrt_PCI_PCPF);



  /* Initialize TR_param */
  TR_param.initialize( R_PCPF );

  /* Compute nominal EI parameter values */
  // NOTE - the nominal theta-range and phi-cross are 0.0 by definition
  //        because the reference point *is* the nominal point.
  TR_ParameterSet  param_set;
  TR_param.compute_params_from_vectors( R_PCPF, V_wrt_PCI_PCPF, param_set );

  /* Compute dispersed EI parameter values */
  param_set.theta_Rng += prm_error[0];
  param_set.phi_Cross += prm_error[1];
  param_set.Vmag      += prm_error[2];
  param_set.gamma     += prm_error[3];
  param_set.Lambda    += prm_error[4];

  /* Compute nominal geodetic altitude */
  jeod::Vector3::copy(R_PCPF, TR_param.pfix_position.cart_coords);
  TR_param.pfix_position.update_from_cart(TR_param.pfix_position.cart_coords);

  /* Define dispersed geodetic altitude */
  double TR_geodetic_altitude = TR_param.pfix_position.ellip_coords.altitude
                              + TR_geodetic_altitude_disp;

  /* Compute dispersed position and velocity vectors */
  //Note: R_PCPF and V_wrt_PCI_PCPF are being reused here
  //      as the dispersed position and velocity vectors.
  //      They were previously the nominal vectors
  //      (a few lines up).
  TR_param.compute_position_from_params( TR_geodetic_altitude,
                                         param_set.theta_Rng,
                                         param_set.phi_Cross,
                                         TargetRelative_StateParameter::INPUT_PHI_ABS_THETA_ROT_LT_90,
                                         R_PCPF);
  TR_param.compute_velocity_from_params( param_set.Vmag,
                                         param_set.gamma,
                                         param_set.Lambda,
                                         R_PCPF,
                                         V_wrt_PCI_PCPF);

  /* Transform dispersed position and velocity into Planet-Centered Inertial */
  double R_PCI[3];
  double V_wrt_PCI_PCI[3];
  jeod::Vector3::transform_transpose(
                   TR_param.pfix_position.planet->pfix.state.rot.T_parent_this,
                   R_PCPF,  //Dispersed position in PCPF
                   R_PCI);  //Dispersed position in PCI
  jeod::Vector3::transform_transpose(
                   TR_param.pfix_position.planet->pfix.state.rot.T_parent_this,
                   V_wrt_PCI_PCPF,  //Dispersed velocity wrt PCI in PCPF
                   V_wrt_PCI_PCI);  //Dispersed velocity wrt PCI in PCI

  /* Compute position and velocity errors */
  for( unsigned int ii = 0; ii < 3; ii++){
    pos_error[ii] = R_PCI[ii]         - position[ii];
    vel_error[ii] = V_wrt_PCI_PCI[ii] - velocity[ii];
  }
}
