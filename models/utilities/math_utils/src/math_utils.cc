/*******************************TRICK HEADER************************************
 PURPOSE: (To provide a common location for additional math utilities)

 PROGRAMMERS:
   (((Gary Turner) (OSR) (Sep 2015) (initial version))
    ((Bingquan Wang) (OSR) (Apr 2017) (Fixed the compilation warnig of
                      float-point number equality comparison)))
    ((Bingquan Wang) (OSR) (May 2017) (cleaned up the code per its
                      IV&V code review))
    ((Daniel Ghan) (OSR) (Jul 2020) (Rearranged files, added log_protected
                      and log10_protected))
    ((Brent Caughron) (OSR) (Oct 2020) (Code review and IV&V)))
*******************************************************************************/

#include <cmath>
#include <cstring> // memcpy, memset
#include <limits>  // min
#include <fenv.h>  // fp exception
#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/math_utils.hh"

/*******************************************************************************
generate_inertial_to_lvlh
Purpose:( Generates the transfromation matrix from inertial to LVLH given
          a position and velocity expressed in inertial.
          LVLH is defined as:
          X - completes
          Y - negative angular momentum
          Z - down)
*******************************************************************************/
void
MathUtils::generate_inertial_to_lvlh( const double position[3],
                                      const double velocity[3],
                                      double T_inrtl_lvlh[3][3])
{
  if (T_inrtl_lvlh == nullptr) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid Output\n",
      "The argument for the output matrix is NULL.\n"
      "Cannot populate a NULL destination.\n"
      "Aborting T_inrtl_lvlh\n");
    return;
  }

  if (position == nullptr ||
      velocity == nullptr) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid arguments\n",
      "The inertial-to-LVLH method cannot function when\n"
      "the input arguments are NULL.\n"
      "Setting transformation matrix to identitiy.\n");
    jeod::Matrix3x3::identity(T_inrtl_lvlh);
    return;
  }

  double pos_mag = jeod::Vector3::vmag(position);

  if ( pos_mag < std::numeric_limits<double>::min() ) {
    CMLMessage::error (
      __FILE__, __LINE__, "Illegal values, LVLH frame undefined:\n",
      "Failed to generate transformation to LVLH for zero state:\n"
      "Aligning LVLH with inertial instead.\n");
    jeod::Matrix3x3::identity( T_inrtl_lvlh);
    return;
  }

  double x_unit[3];
  double y_unit[3];
  double z_unit[3];

  jeod::Vector3::scale( position, (-1/pos_mag), z_unit);

  // Generate the - angular momentum unit vector
  // NOTE - The following is (v x r) not (r x v) as is normal for angular
  // momentum. This is so that it becomes negative for creating y_unit.
  // Which would be defined as (-h/hmag), where h is angular momentum.
  jeod::Vector3::cross(velocity, position, y_unit);

  double y_mag = jeod::Vector3::vmag(y_unit);
  if ( y_mag >=std::numeric_limits<double>::min() ) {
    jeod::Vector3::scale ( (1/y_mag), y_unit);
    jeod::Vector3::cross( y_unit, z_unit, x_unit );
  }
  else { // Not likely, but possible
    // The velocity is radial or zero; there is no angular momentum.
    // Frame is not well defined.  Send a warning.
    // z-axis is still defined, but not x and y.
    // unless the position lies on the inertial x-axis, align the LVLH x-axis
    // in the same sense as the inertial x-axis while retaining perpendicular
    // to LVLH z-axis.  LVLH-y completes.
    // If the position is on the inertial x-axis, that doesn't work.  in that
    // case, align the LVLH y-axis in the same sense as the inertial y-axis
    // while retaining perpendicular to LVLH z-axis.  LVLH-x completes.

    if (std::fabs(position[1]) >= std::numeric_limits<double>::min() ||
        std::fabs(position[2]) >= std::numeric_limits<double>::min()) {
      CMLMessage::warn (
        __FILE__, __LINE__, "LVLH frame undefined:\n",
        "Failed to generate transformation to LVLH for state with\n"
        "zero angular-momentum.\n"
        "Aligning LVLH-x close to inertial-x.\n");
      // Try y_lvlh = ( (x_inrtl) X (r_inrtl) ).
      // This is identical to ( (z_lvlh) X (x_inrtl) )
      // Then x_lvlh = y_lvlh x z_lvlh  oriented with x_inrtl
      y_unit[0] =  0.0;
      y_unit[1] = -position[2];
      y_unit[2] =  position[1];

      jeod::Vector3::cross( y_unit, z_unit, x_unit );
    }
    else {
      CMLMessage::warn (
        __FILE__, __LINE__, "LVLH frame undefined:\n",
        "Failed to generate transformation to LVLH for state with\n"
        "zero angular-momentum.\n"
        "Aligning LVLH-y close to inertial-y.\n");
      x_unit[0] = -position[2];
      x_unit[1] =  0.0;
      x_unit[2] =  position[0];

      jeod::Vector3::cross( z_unit, x_unit, y_unit );
    }
    jeod::Vector3::normalize( y_unit);
  }

  jeod::Vector3::normalize(x_unit);

  for (unsigned int ii = 0; ii < 3; ++ii) {
    T_inrtl_lvlh[0][ii] = x_unit[ii];
    T_inrtl_lvlh[1][ii] = y_unit[ii];
    T_inrtl_lvlh[2][ii] = z_unit[ii];
  }
}

/*******************************************************************************
generate_inertial_to_uvw
Purpose:( Generates the transformation matrix from inertial to UVW given
          a position and velocity expressed in inertial.
          UVW is defined as:
          U - Up
          V - completes
          W - orbital angular momentum)
*******************************************************************************/
void
MathUtils::generate_inertial_to_uvw( const double position[3],
                                     const double velocity[3],
                                     double T_inrtl_uvw[3][3])
{
  if (T_inrtl_uvw == nullptr) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid Output\n",
      "The argument for the output matrix is NULL.\n"
      "Cannot populate a NULL destination.\n"
      "Aborting T_inrtl_uvw\n");
    return;
  }

  if (position == nullptr ||
      velocity == nullptr) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid arguments\n",
      "The inertial-to-UVW method cannot function when\n"
      "the input arguments are NULL.\n"
      "Setting transformation matrix to identitiy.\n");
    jeod::Matrix3x3::identity(T_inrtl_uvw);
    return;
  }

  double pos_mag = jeod::Vector3::vmag(position);

  if ( pos_mag < std::numeric_limits<double>::min() ) {
    CMLMessage::error (
      __FILE__, __LINE__, "Illegal values, UVW frame undefined:\n",
      "Failed to generate transformation to UVW for zero state:\n"
      "Aligning UVW with inertial instead.\n");
    jeod::Matrix3x3::identity( T_inrtl_uvw);
    return;
  }

  double u_unit[3];
  double v_unit[3];
  double w_unit[3];

  jeod::Vector3::scale(position, (1/pos_mag), u_unit);

  jeod::Vector3::cross( position, velocity, w_unit);
  double w_mag = jeod::Vector3::vmag(w_unit);
  if ( w_mag >= std::numeric_limits<double>::min() ) {
    jeod::Vector3::scale ( (1/w_mag), w_unit);
    jeod::Vector3::cross( w_unit, u_unit, v_unit );
  }
  else { // Not likely, but possible
    // The velocity is radial or zero; there is no angular momentum.
    // Frame is not well defined.  Send a warning.
    // u-axis is still defined, but not v and w.
    // unless the position lies on the inertial z-axis, align the UVW w-axis
    // in the same sense as the inertial z-axis while retaining perpendicular
    // to UVW u-axis.  UVW-v completes.
    // If the position is on the inertial z-axis, that doesn't work.  in that
    // case, align the UVW w-axis in the same sense as the inertial x-axis
    // while retaining perpendicular to UVW u-axis.  UVW-v completes.

    if (std::fabs(position[0]) >= std::numeric_limits<double>::min() ||
        std::fabs(position[1]) >= std::numeric_limits<double>::min() ) {
        // i.e. not on z-axis
      CMLMessage::warn (
        __FILE__, __LINE__, "UVW frame undefined:\n",
        "Failed to generate transformation to UVW for state with\n"
        "zero angular-momentum.\n"
        "Aligning UVW-w close to inertial-z.\n");
      // Try v_uvw = ( (z_inrtl) X (r_inrtl) ).
      // This is identical to ( (z_inrtl) X (u_uvw) )
      // Then w_uvw = u_uvw x v_uvw  oriented with z_inrtl
      v_unit[0] = -position[1];
      v_unit[1] =  position[0];
      v_unit[2] =  0.0;

      jeod::Vector3::cross( u_unit, v_unit, w_unit );
    }
    else { // i.e. velocity is radial AND along z-axis
      CMLMessage::warn (
        __FILE__, __LINE__, "UVW frame undefined:\n",
        "Failed to generate transformation to UVW for state with\n"
        "zero angular-momentum.\n"
        "Aligning UVW-w close to inertial-x.\n");
      // Try v_uvw = ( (x_inrtl) X (r_inrtl) ).
      // This is identical to ( (x_inrtl) X (u_uvw) )
      // Then w_uvw = u_uvw x v_uvw  oriented with x_inrtl
      v_unit[0] = 0.0;
      v_unit[1] = -position[2];
      v_unit[2] =  position[1];

    }
    jeod::Vector3::cross( u_unit, v_unit, w_unit );
    jeod::Vector3::normalize( w_unit);
  }
  jeod::Vector3::normalize( v_unit);
  for (unsigned int ii = 0; ii < 3; ++ii) {
    T_inrtl_uvw[0][ii] = u_unit[ii];
    T_inrtl_uvw[1][ii] = v_unit[ii];
    T_inrtl_uvw[2][ii] = w_unit[ii];
  }
}

/*******************************************************************************
generate_inertial_to_reference
Purpose:( Generate the transformation matrix between the inertial reference
          frame and the reference reference-frame given the x-axis in inertial
          space and the position vector of the reference frame relative to the
          initializing planet.

          Frame definition is:
          x-axis as specified
          y-axis x-axis cross position
          z-axis completes.

          This definition puts the z-axis in the same plane as the position
          vector and the defined x-axis, generally aligned in the opposite
          sense as the position vector while remaining perpendicular to the
          defined x-axis)
Assumption: (Input vector is expressed in inertial)
*******************************************************************************/
void
MathUtils::generate_inrtl_to_reference( const double x_axis_inrtl[3],
                                        const double position[3],
                                        double T_inrtl_reference[3][3])
{
  if (T_inrtl_reference == nullptr) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid Output\n",
      "The argument for the output matrix is NULL.\n"
      "Cannot populate a NULL destination.\n"
      "Aborting T_inrtl_reference\n");
    return;
  }

  if (x_axis_inrtl == nullptr ||
      position == nullptr) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid arguments\n",
      "The inertial-to-reference method cannot function when\n"
      "the input arguments are NULL.\n"
      "Setting transformation matrix to identity.\n");
    jeod::Matrix3x3::identity(T_inrtl_reference);
    return;
  }

  double x_mag = jeod::Vector3::vmag(x_axis_inrtl);
  if (jeod::Vector3::vmag(position) < std::numeric_limits<double>::min() ||
      x_mag < std::numeric_limits<double>::min() ) {
    CMLMessage::error (
      __FILE__, __LINE__,
      "Illegal values, reference ref-frame not definable:\n",
      "Tried to create a position--x-axis frame\n"
      "but either the position or x-axis is a zero vector\n"
      "Aligning reference ref-frame with inertial frame instead.\n");
    jeod::Matrix3x3::identity( T_inrtl_reference);
    return;
  }

  // Make sure that the x-axis vector is a unit vector
  // Note - in the event of a zero-vector, normalize returns a zero-vector.
  double x_unit[3];
  jeod::Vector3::scale( x_axis_inrtl, (1/x_mag), x_unit);

  double y_unit[3];
  jeod::Vector3::cross( x_unit,
                  position,
                  y_unit); // not unit vec (yet)

  double y_axis_mag = jeod::Vector3::vmag(y_unit);

  // If x-axis and position are aligned, cross product goes away.
  // In this case, use the inertial z-axis in place of the position vector.
  // This will make the new z-axis align in the same general sense as the
  // inertial z-axis
  // then the y-axis lies in the inertial x-y plane.

  // As a last resort, if the position vector lies along the inertial z-axis.
  // Align new y-axis with inertial y-axis, new z-axis with -inrtl-x-axis
  if (y_axis_mag < std::numeric_limits<double>::min()) {
    if ( std::fabs(x_unit[0]) >= std::numeric_limits<double>::min() ||
         std::fabs(x_unit[1]) >= std::numeric_limits<double>::min()) {
      y_unit[0] =  x_unit[1];
      y_unit[1] = -x_unit[0];
      y_unit[2] =  0.0;
      y_axis_mag = jeod::Vector3::vmag(y_unit);
    }
    else {
      y_unit[1]  = 1.0;
      y_axis_mag = 1.0;
    }
  }

  // Now turn y_unit into a unit vector
  jeod::Vector3::scale((1/y_axis_mag), y_unit);

  double z_unit[3];
  jeod::Vector3::cross(x_unit, y_unit, z_unit);

  // y_unit and x_unit are perpendicular.  But just to be sure,
  // normalize z-unit anyway.
  jeod::Vector3::normalize(z_unit);

  // T_reference_to_inertial =  [ x0 y0 z0]
  //                          [ [ x1 y1 z1] ]
  //                            [ x2 y2 z2]
  // T_inrtl_reference =  [ x0 x1 x2]
  //                    [ [ y0 y1 y2] ]
  //                      [ z0 z1 z2]]
  for (unsigned int ii = 0; ii < 3; ii++) {
    T_inrtl_reference[0][ii] = x_unit[ii];
    T_inrtl_reference[1][ii] = y_unit[ii];
    T_inrtl_reference[2][ii] = z_unit[ii];
  }
}

/*******************************************************************************
generate_inertial_to_vnc
Purpose:( Generate the transformation matrix between the inertial reference
          frame and the VNC reference-frame given the position and velocity
          vectors in inertial space.

          Frame definition is:
          x-axis (V) along velocity vector
          y-axis (N) along angular momentum vector.
          z-axis (C) completes (in orbital plane, generally along r vector)

Assumption: (Input vector is expressed in inertial)
*******************************************************************************/
void
MathUtils::generate_inrtl_to_vnc( const double (&position)[3],
                                  const double (&velocity)[3],
                                  double (&T_inrtl_vnc)[3][3])
{
  double vel_mag =  jeod::Vector3::vmag(velocity);
  if (jeod::Vector3::vmag(position) < std::numeric_limits<double>::min() ||
      vel_mag                 < std::numeric_limits<double>::min()) {
    CMLMessage::error ( __FILE__, __LINE__,
      "Illegal values, reference ref-frame not definable:\n"
      "Tried to create a VNC frame\n"
      "but either the position or velocity is a zero vector\n"
      "Aligning VNC ref-frame with inertial frame instead.\n");
    jeod::Matrix3x3::identity( T_inrtl_vnc);
    return;
  }

  // Make sure that the x-axis vector is a unit vector
  // Note - in the event of a zero-vector, normalize returns a zero-vector.
  double x_unit[3];
  jeod::Vector3::scale( velocity, (1/vel_mag), x_unit);

  double y_unit[3];
  jeod::Vector3::cross( position,
                  velocity,
                  y_unit); // not unit vec (yet)

  double y_axis_mag = jeod::Vector3::vmag(y_unit);
  if ( y_axis_mag  < std::numeric_limits<double>::min()) {
    CMLMessage::error ( __FILE__, __LINE__,
      "Illegal values, VNC ref-frame not definable:\n"
      "Tried to create a VNC frame\n"
      "but the position and velocity are aligned.\n"
      "Aligning VNC ref-frame with inertial frame instead.\n");
    jeod::Matrix3x3::identity( T_inrtl_vnc);
    return;
  }

  // Now turn y_unit into a unit vector
  jeod::Vector3::scale((1/y_axis_mag), y_unit);

  double z_unit[3];
  jeod::Vector3::cross(x_unit, y_unit, z_unit);

  // y_unit and x_unit are perpendicular.  But just to be sure,
  // normalize z-unit anyway.
  jeod::Vector3::normalize(z_unit);

  // T_vnc_to_inertial =  [ x0 y0 z0]
  //                    [ [ x1 y1 z1] ]
  //                      [ x2 y2 z2]
  // T_inrtl_vnc =  [ x0 x1 x2]
  //              [ [ y0 y1 y2] ]
  //                [ z0 z1 z2]]
  for (unsigned int ii = 0; ii < 3; ii++) {
    T_inrtl_vnc[0][ii] = x_unit[ii];
    T_inrtl_vnc[1][ii] = y_unit[ii];
    T_inrtl_vnc[2][ii] = z_unit[ii];
  }
}

/*****************************************************************************
generate_T_pfix_to_enu
Purpose:
  Generate the transformation matrix between the planet-fixed reference
  frame and the TOPOCENTRIC East-North-Up reference-frame with origin
  at the specified position.

  2 arguments:
    - the reference position vector, expressed in Cartesian Pfix (input)
    - the transformation matrix from Pfix to TC-ENU (output)

  Up    ([2]) is the direction of the specified position vector
  East  ([0]) is the cross product of Pfix-z with up
  North ([1]) is the cross product of up with east.

Assumptions:
  - Input vector is expressed in Cartesian Pfix
  - ENU frame is the TOPCENTRIC ENU frame.
    - THIS WILL NOT GENERATE A TOPODETIC ENU FRAME.
*****************************************************************************/
void
MathUtils::generate_T_pfix_to_enu( const double position_pfix[3],
                                   double T_pfix_to_enu[3][3])
{
  if (T_pfix_to_enu == nullptr) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid Output\n",
      "The argument for the output matrix is NULL.\n"
      "Cannot populate a NULL destination.\n"
      "Aborting T_pfix_to_enu\n");
    return;
  }

  if( position_pfix == nullptr) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid Position\n",
      "Position vector is NULL.\n"
      "Cannot generate the ENU frame.\n"
      "Setting transformation matrix to identitiy.\n");
    jeod::Matrix3x3::identity(T_pfix_to_enu);
    return;
  }

  // Create 3-arrays to express the 3 axes of ENU in the ECEF frame
  double east_hat[3]={0};
  double north_hat[3]={0};
  double up_hat[3]={0};

  // Up is the position vector, passed in as an argument.  Need to normalize
  // this vector to get a unit-vector.
  jeod::Vector3::normalize( position_pfix, up_hat);

  // Check for polar positions -- east and north not defined:
  if (MathUtils::is_near_equal( std::abs(up_hat[2]), 1.0)) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid Position\n",
      "For polar positions, East and north are not defined.\n"
      "Aligning east with pfix +x\n"
      "         north to complete.\n");
    east_hat[0] = 1.0;
    north_hat[1] = up_hat[2] = (up_hat[2]>0) ? 1.0 : -1.0;
  }
  else {
    // East unit-vector is the normalization of (ECEF-z) x (up).
    east_hat[0] = -up_hat[1];
    east_hat[1] =  up_hat[0];
    east_hat[2] = 0.0;
    jeod::Vector3::normalize(east_hat);

    // North unit-vector is (up) x (east). Normalization is not necessary here,
    // but safer to do it anyway, just in case.
    jeod::Vector3::cross( up_hat, east_hat, north_hat);
    jeod::Vector3::normalize(north_hat);
  }

  // Construct the matrix:
  //                    [ e_x    e_y     e_z]
  //  T_pfix_to_enu =   [ n_x    n_y     n_z]
  //                    [ u_x    u_y     u_z]

  jeod::Vector3::copy( east_hat, T_pfix_to_enu[0]);
  jeod::Vector3::copy( north_hat, T_pfix_to_enu[1]);
  jeod::Vector3::copy( up_hat, T_pfix_to_enu[2]);
}

/*****************************************************************************
generate_Q_enu_to_pfix
Purpose:
  Generate the transformation matrix between the planet-fixed reference
  frame and the GENERIC East-North-Up reference-frame with origin
  at the specified position.

  3 arguments:
    - longitude in radians (input)
    - latitude in radians (input)
    - the left-transformation quaternion from Pfix to ENU (output)


Assumptions:
  - The desired ENU frame is determined by the choice of latitude -- be it
    topodetic or topocentric
    - if a topodetic latitude is provided, the quaternion will be from
      topodetic-ENU to Pfix
    - if a topocentric latitude is provided, the quaternion will be from
      topocentric-ENU to Pfix
*****************************************************************************/
void
MathUtils::generate_Q_enu_to_pfix( double longitude,
                                   double latitude,
                                   jeod::Quaternion & Q_enu_to_pfix)
{
  if (latitude > M_PI_2) {
      CMLMessage::warn(
        __FILE__,__LINE__, "Latitude too large\n",
        "Input latitude is greater than 90 Degrees, which would rotate you \n"
        "past the North Pole onto the other side of the planet. \n"
        "Latitude will be set to ", M_PI, " - ", latitude, " and the Longitude will be increased"
        " by ", M_PI, ".\n");
      latitude = M_PI - latitude;
      longitude += M_PI;
  } else if (latitude < -M_PI_2) {
      CMLMessage::warn(
        __FILE__,__LINE__, "Latitude too small\n",
        "Input latitude is greater than -90 Degrees, which would rotate you \n"
        "past the South Pole onto the other side of the planet. \n"
        "Latitude will be set to -", M_PI, " - ", latitude, " and the Longitude will be increased"
        " by ", M_PI, ".\n");
      latitude = -M_PI - latitude;
      longitude += M_PI;
  }


  jeod::Quaternion Q_enu_to_uen;
  Q_enu_to_uen.scalar    =
  Q_enu_to_uen.vector[0] =
  Q_enu_to_uen.vector[1] =
  Q_enu_to_uen.vector[2] = 0.5;

  // equatorial East-y frame (equEy) is the rotation of the UEN frame such that
  // x and y are parallel to the equatorial plane (x is "up", y is
  // "Equatorial-East"), and z is parallel to the pfix z-axis.
  jeod::Quaternion Q_uen_to_equEy; // constructs to [1,[0,0,0]]
  Q_uen_to_equEy.scalar = std::cos( latitude/2);
  Q_uen_to_equEy.vector[1] = -std::sin( latitude/2);

  jeod::Quaternion Q_equEy_to_pfix; // constructs to [1,[0,0,0]]
  Q_equEy_to_pfix.scalar = std::cos( longitude/2);
  Q_equEy_to_pfix.vector[2] = std::sin( longitude/2);

  jeod::Quaternion Q_uen_to_pfix;
  Q_equEy_to_pfix.multiply( Q_uen_to_equEy,
                            Q_uen_to_pfix);
  Q_uen_to_pfix.multiply( Q_enu_to_uen,
                          Q_enu_to_pfix);
};

/*******************************************************************************
polynomial
Purpose:( Evaluates a polynomial y= Sigma a_i x^i given a vector of
          coefficients and the variable x.)
*******************************************************************************/
double
MathUtils::polynomial( double x,
                       std::vector<double> & coeffs,
                       const double failed_val,
                       const bool   failed_flag)
{
  // Temporary disable fp exceptions, storing the set of
  // previously configured exceptions.
  const int fe_prev = fedisableexcept(FE_ALL_EXCEPT);
  assert(-1 != fe_prev);  // If -1, there was a failure

  double x_to_i = 1.0;
  double sum = 0.0;
  for (std::vector< double >::iterator it = coeffs.begin();
       it != coeffs.end();
       ++it) {
    sum += ((*it) * x_to_i);
    x_to_i *= x;
  }

  if (std::isnan(sum) || std::isinf(sum)) {  //chec invalid ops and overflow
    if (failed_flag) {
      CMLMessage::fail(
        __FILE__, __LINE__,"Overflow value detected.\n",
        "The polynomial sum is overflow.\n"
        "Terminating execution.\n");
    }
    else {
      CMLMessage::warn(
        __FILE__, __LINE__,"Overflow value detected.\n",
        "The polynomial sum is overflow,"
        " and the result is set as ", failed_val, ".6e.\n");
    }
    sum = failed_val;
  }
  feenableexcept(fe_prev); // restore the previous settings of fp exceptions
  return sum;
}

/*******************************************************************************
cholesky_decomposition
Purpose:( Effectively takes the square root of a matrix such that
                S * S' = T
          where S is the square-root matrix and T is the original matrix
          - Assumes that T is symmetric (checks this)
          - Assumes that T is found in the upper-left square of the data array
            that is passed in
          - S is a lower-diagonal matrix, the upper diagonal values are all zero
          - out_array will be populated directly; it is assumed that it is large
            enough to hold the generated array.)
*******************************************************************************/
bool
MathUtils::cholesky_decomposition ( std::string origin,
                                    const double * in_array,
                                    double * out_array,
                                    size_t   dimension_in,
                                    size_t   sub_mx_size)
{
  if (in_array == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid inputs\n",
      "The array containing the matrix to be decomposed is addressed with a "
      "NULL pointer.\nThere is no data to apply the method to.\n"
      "Aborting decomposition\n");
    return false;
  }
  if (out_array == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid inputs\n",
      "The array to which the computed decomposition will be sent is "
      "addressed with a NULL pointer.\n"
      "There is nowhere to send the result.\n"
      "Aborting decomposition\n");
    return false;
  }
  // NOTES
  // - "dimension_in" is typically the length of 1 side of the (square)
  //   matrix "in_array", the array that will be decomposed.
  //   However, it is not required that the entirety of "in_array" be
  //   decomposed.
  //   It is assumed that the target matrix occupies the top left
  //   "sub_mx_size" x "sub_mx_size" elements from "in_array"
  //   and that "dimension_in" represents the number of elements PER ROW
  //   of array_in.
  //   e.g. to decompose the top 2x2 matrix of
  //              [ 11  12  13 ]
  //        A =   [ 21  22  23 ]
  //              [ 31  32  33 ]
  //        call this method as follows:
  //          cholesky_decomposition( A,
  //                                  B,
  //                                  3,
  //                                  2);
  //    This method does not support decomposition of any other
  //    submatrix, and it is assumed that in_array is at least as large
  //    as "dimension_in" columns x "sub_mx_size" rows
  //
  // - If "sub_mx_size" is not specified, it is set equal to dimension_in
  //   and the entire "dimension_in" x "dimension_in" array is decomposed;
  //   this is the default behavior.


  // sub_mx_size is an optional argument.  If unspecified, it takes the
  // value of dimension_in
  if (sub_mx_size == 0) {
    sub_mx_size = dimension_in;
  }
  else if( sub_mx_size > dimension_in) { // in-data is too small!!
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid sub-matrix specification\n",
      "The input matrix is declared to be size ", dimension_in, ", and the request has been\n"
      "made to use a sub-matrix of this input, with size ", sub_mx_size, ".\n"
      "A sub-matrix must be smaller than the original matrix.\n"
      "Aborting decomposition.\n");
    return false;
  }


  // Data comes in as a 1-d array; need to set this up as a 2-d array.
  // The incoming array might be a larger data array, containing the
  // matrix and other bits and pieces.  If it is, just copy it anyway and
  // then only use those bits of it that are needed; this is (probably)
  // faster than if-blocks and copying row-by-row.

  double  input_mx[dimension_in][dimension_in];
  memcpy( input_mx, in_array, sizeof(input_mx));

  // Perform quick check of symmetry
  bool is_symmetric = true;
  for (size_t ii = 0; ii < sub_mx_size - 1; ++ii) {
    for (size_t jj = ii+1; jj < sub_mx_size; ++jj) {
      // Check that the upper diagonal element ([ii][jj]) is either zero
      // or matches the lower diagonal element and drop a warning if it does
      // not.
      if ( !MathUtils::is_near_equal( input_mx[ii][jj],
                                      input_mx[jj][ii]) &&
           !MathUtils::is_near_equal( input_mx[ii][jj],
                                      0.0)) {
        CMLMessage::error(
          __FILE__, __LINE__, "Non-symmetric matrix\n",
          "Matrix input from ", origin, " is not a symmetric matrix.\n"
          "Decomposition will proceed as though it was, but will use only\n"
          "the values on and below the diagonal.\n"
          "Values above the diagonal will be ignored, assumed equal to those "
          "below the diagonal.\n");
        is_symmetric = false;
        break;
      }
    }
    if (!is_symmetric) break;
  }

  // sqrt_mx will be sized to match in_array; its contents will be the
  //           desired output in the top left corner and 0 elsewhere.
  double sqrt_mx[dimension_in][dimension_in];
  memset( sqrt_mx, 0, sizeof(sqrt_mx));

  // Now process input_mx and generate out_local
  for (size_t ii = 0; ii < sub_mx_size; ii++) {
    // start with the diagonal element
    double sum = input_mx[ii][ii];
    // subtract off the square of the sqrt_matrix elements for all
    // elements to the left of this diagonal element.
    // These have already been computed; all elements to the left of the
    // current diagonal element (ii) were processed with previous values
    // of (ii) -- for each value of (ii), the sqrt_matrix values are computed
    // for the corresponding diagonal element and all elements below it
    // (i.e. those in the same column).
    for (size_t jj = 0; jj < ii; jj++) {
      sum -= (sqrt_mx[ii][jj] * sqrt_mx[ii][jj]);
    }
    // If the sub-matrix of in_array is well-formed, this sum will be >= 0.0
    // Otherwise it cannot be used.
    if (sum < 0.0){
      CMLMessage::error(
        __FILE__, __LINE__, "Invalid Covariance\n",
        "Matrix input from ", origin, " is not positive semi-definite.\n"
        "A negative eigen-value exists, detected in processing col ", ii, "\n"
        "Successful decomposition requires the matrix be positive "
        "semi-definite.\nDecomposition failed.\n");
      return false;
    }

    // if "sum" is equal to zero, we are about to set the sqrt_mx value at
    // position [ii][ii] to 0.0.
    // To generate the values below the diagonal element at position [ii][ii],
    // we have to divide by this value ... which means dividing by zero.
    // So, for example, sqrt_mx[3][2] = f_3 / sqrt_mx[2][2] = f_3 / 0.0
    //   - If f != 0.0, this fails.
    //   - If f == 0.0, the result is ambiguous and we (arbitrarily) decide that
    //     it will have the value 0.0
    //     (note - this best supports the most common cause of sum = 0.0,
    //             which is that, for all values jj,
    //             input_mx[jj][ii] = input_mx[ii][jj]  = 0.0)
    // So if sum = 0.0, we need to evaluate f_jj for all jj > ii
    //    if f_jj != 0.0, the decomposition fails.
    //    f_jj = input_mx[jj][ii] -
    //           sigma_{kk=0:ii-1} ( sqrt_mx[jj][kk] * sqrt_mx[ii][kk])
    if (MathUtils::is_near_equal(sum, 0.0)) {
      for (unsigned int jj = ii+1; jj < sub_mx_size; jj++) {
        sum = input_mx[jj][ii];
        for (unsigned int kk = 0; kk < ii; kk++) {
          sum -= (sqrt_mx[jj][kk] * sqrt_mx[ii][kk]);
        }
        if (!MathUtils::is_near_equal( sum, 0.0)) {
          CMLMessage::error(
            __FILE__, __LINE__, "Invalid Covariance\n",
            "Matrix input from ", origin, " is not positive semi-definite.\n"
            "A negative eigen-value exists\n"
            "Successful decomposition requires the matrix be positive "
            "semi-definite.\nDecomposition failed.\n");
          return false;
        }
      }
      // If still executing here, all elements below [ii][ii] have the
      // appropriate value to generate an ambiguous solution.
      // Leave all the sqrt_mx values in that column at 0.0 (sqrt_mx was
      // constructed with 0.0 everywhere) and move on to the next column.
      CMLMessage::warn(
        __FILE__,__LINE__, "Ambiguous Decomposition\n",
        "Matrix input from ", origin, ":\n"
        "Consider the ", ii+1, "x", ii+1, " square sub-matrix taken from the upper left of "
        "the input matrix.\nThis sub-matrix has a determinant "
        "equal to zero (or very close to zero).\n"
        "There are multiple solutions for the decomposition of this matrix.\n"
        "Selecting the solution in which the values in column [", ii, "] are zeros."
        "\n");
      continue; // to next ii
    }
    // else: compute the values for this diagonal element and all elements
    // below it:
    sqrt_mx[ii][ii] = std::sqrt( sum );

    // Now generate the sqrt_cov values for the off-diagonal elements below
    // the current diagonal element (we are still inside the (ii) loop).
    // Moving down the rows one at a time starting with the row below the
    // current diagonal element and progressing all the way to the bottom:
    for (unsigned int jj = ii+1; jj < sub_mx_size; jj++) {
      // start with the specified input_mx at this element and subtract off
      // the product of the sqrt_mx values (already computed) corresponding to:
      //  -- the element in row (ii), and
      //  -- the element in this row
      // for every column to the left of the current column.
      // This is analgous to subtracting off the scalar product of the row(ii)
      // and this row for elements to the left.
      // These values have already been computed because values are computed
      // for all rows as each column is processed, moving to the right.
      sum = input_mx[jj][ii];
      for (unsigned int kk = 0; kk < ii; kk++) {
        sum -= (sqrt_mx[jj][kk] * sqrt_mx[ii][kk]);
      }
      // All elements in this column are represented by their respective sum
      // scaled by a common value.  That value is equal to the value of the
      // diagonal element at the top of this column (of lower-diagonal
      // elements).  Note that this is true even for the diagonal element
      // itself, which is the square root of its "sum" value.
      sqrt_mx[jj][ii] = sum / sqrt_mx[ii][ii];
    }
  } // processed an array with dimension:  sub_mx_size x sub_mx_size
    // Any other elements remain zero.

  // Copy the generated matrix back to the memory space passed in and return
  // true to indicate that the operation was successful.
  memcpy( out_array, sqrt_mx, sizeof(sqrt_mx));
  return true;
}

/*******************************************************************************
compute_backward_difference
Purpose:( General mathematical tool for generating backward difference
          derivatives.)
Limitations:
  This method returns an averaged difference, not a derivative. The return
  value is something like: (dy / dt) * (DELTA t);
  It is the responsibility of the calling function to generate and maintain
  the history list and divide the result by (DELTA-t) if a derivative value
  is desired.
Assumptions:
  - the historical points are equally spaced and
  - the front of the list is the most recent data point (and therefore
  - the back of the list is the oldest data point
*******************************************************************************/
double
MathUtils::compute_backward_difference( const std::list<double> & history)
{
  if (history.empty()) {
    CMLMessage::error(
      __FILE__,__LINE__,"invalid data support\n",
      "there is no history for this variable.\n"
      "Returning backward difference of 0.0.\n");
    return 0.0;
  }

  static const std::array<std::array<double, 5>, 5> back_diff_coefficients =
    {{{     0,     0,    0,    0,     0   },
      {     1,    -1.0,  0,    0,     0   },
      {   1.5,    -2.0,  0.5,  0,     0   },
      {  11.0/6,  -3.0,  1.5, -1.0/3, 0   },
      {  25.0/12, -4.0,  3.0, -4.0/3, 0.25}}};
  double derivative = 0.0;
  size_t order = std::min(history.size() - 1, (size_t)4);

  size_t ii = 0;
  for (std::list<double>::const_iterator it = history.begin();
       it != history.end() && ii < 5;
       ++it) {

    derivative += (*it) * back_diff_coefficients[order][ii];
    ++ii;
  }
  return derivative;
}



/*****************************************************************************
compute_unit_vector_derivative
Purpose:
  Computes the derivative of the unit-vector -- note not the normalized
  derivative vector, this is the rate at which the unit vector changes.
  See documentation.
*****************************************************************************/
void
MathUtils::compute_unit_vector_derivative(
    const double vector[3],                // input
    const double vector_derivative[3],     // input
    double unit_vector_derivative[3])      // output
{
  double vdotv = jeod::Vector3::dot( vector,
                               vector);
  if (is_near_equal( vdotv, 0.0)) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid inputs to method.\n",
      "A unit vector is not defined for a zero vector, and the derivative\n"
      "of an undefined vector is also undefined.\n"
      "Normalizing the vector-derivative as an alternative to the "
      "unit-vector-derivative.\n");
    jeod::Vector3::normalize( vector_derivative,
                        unit_vector_derivative);
    return;
  }
  double vdotvd = jeod::Vector3::dot( vector,
                                vector_derivative);

  // Not protected by previous test of vdotv != 0.0;
  // if vdotv is small and vdotvd large, large / small could still overflow.
  // So use divide-protected.
  double vdotvd_over_vdotv = divide_protected( vdotvd,
                                               vdotv,
                                               0,
                                               true);
  double scratch[3];
  jeod::Vector3::scale(  vector,
                  -vdotvd_over_vdotv,
                   scratch);

  jeod::Vector3::incr( vector_derivative,
                 scratch);

  // Protected by previous verification that vdotv not close to 0.
  double inv_sqrt_vdotv = 1 / std::sqrt( vdotv);

  // NOTE - not really protected, but failure here would require that scratch be
  //        very large and vdotv very small -- effectively the vector is very
  //        small with a very large derivative perpendicular to it.
  jeod::Vector3::scale( scratch,
                  inv_sqrt_vdotv,
                  unit_vector_derivative);
}

/*****************************************************************************
Name: is_equal
Purpose:
  Template specializations for float and double to avoid use of ==
  Uses "has_changed_from" under the presumption that one of the two values
  represents a previously-assigned value.
*****************************************************************************/
template<>
bool MathUtils::is_equal<float>( float val1, float val2)
{
  return !has_changed_from( val1, val2);
}
/****************************************************************************/
template<>
bool MathUtils::is_equal<double>( double val1, double val2)
{
  return !has_changed_from( val1, val2);
}


/*****************************************************************************
Name: is_within_abs_tolerance
Purpose:
    Template specialization for bool type
*****************************************************************************/
template<>
bool MathUtils::is_within_abs_tolerance<bool>( bool v1, bool v2, bool tol)
{
  bool ret = tol || (v1 == v2);
  CMLMessage::error( __FILE__, __LINE__,
    "Testing whether boolean ",v1, " is within boolean ",tol,", of boolean ",
    v2, " has an ambiguous interpretation.\n"
    "Evaluated to ",ret,"\n");
  return ret;
}
