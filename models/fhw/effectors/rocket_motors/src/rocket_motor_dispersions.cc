/*******************************************************************************
PURPOSE:
   (Provide a mechanism for dispersing the values associated with position
    and attitude of a rocket motor.
    This typically represents errors introduced in machining.)

LIBRARY DEPENDENCIES:
   ()

PROGRAMMERS:
   (((Brenton Caughron, Gary Turner) (OSR) (May 2018) (Antares) (initial)))
*******************************************************************************/
#include <cstddef> // NULL
#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/rocket_motor_dispersions.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
RocketMotorDispersions::RocketMotorDispersions()
  :
  position_dispersion{0.0, 0.0, 0.0},
  motor_tolerance{0.0, 0.0, 0.0},
  tolerance_mag_threshold(1.0E-12),
  T_dispersion_to_nominal{{1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{0.0, 0.0, 1.0}}
{}

/*****************************************************************************
apply_dispersions
Purpose:(Disperse the position and attitude of the motor frame. The nominal
         transformation matrix from the parent frame (often the structural
         frame) to the motor frame is provided as an input and is
         most frequently identity.
         The method allows dispersion on that nominal frame-state to model
         machining error, etc.)
*****************************************************************************/
void
RocketMotorDispersions::apply_dispersions(
    double position[3],
    double T_parent_to_motor[3][3])
{
  // The attitude-dispersion vector (i.e. motor_tolerance) and
  // position-dispersion vector should be pseudo-randomly generated values
  // bounded by the spec. tolerances for the attitude and position of the motor
  // frame.
  // If the axes are correlated, that correlation should be included in the
  // generation of these dispersions; this model has no knowledge of the
  // bounds or correlations associated with tolerances, and has no knowledge
  // or responsibility for the generation of these dispersions.

  if (position == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid calling argument\n",
      "Received position vector is NULL.\n"
      "Cannot apply dispersion to a NULL vector.\n");
    return;
  }
  // Add the position dispersion (trivial):
  jeod::Vector3::incr( position_dispersion,
                 position);

  if (T_parent_to_motor == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid calling argument\n",
      "Received transformation matrix is NULL.\n"
      "Cannot apply dispersion to a NULL matrix.\n");
    return;
  }
 
  // Add the attitude dispersion (more complex):
  // This is based on Rodrigues' formula
  double tolerance_mag = jeod::Vector3::vmag( motor_tolerance);

  // If the dispersion is too small to be significant, don't do anything, return
  // leaving the transformation matrix unchanged.
  // Note "too small to be significant" is at user discretion and determined
  // by the value of tolerance_mag_threshold.
  if (tolerance_mag < tolerance_mag_threshold) {
    return;
  }
  // Now use Rodrigues' formula, which provides the rotation matrix
  // that represents a rotation through some angle theta about some
  // unit-vector n-hat.
  // The rotation matrix provided by Rodrigues' formula rotates a vector, r,
  // such that the new value:
  //   r' = T r
  // We will be using it to generate a transformation matrix between the nominal
  // frame and the rotated frame. T in this expression represents the
  // transformation from rotated-frame to nominal-frame (see documentation).
  //
  // There are 2 forms of this formula:
  //    T = I           + S sin-theta + S^2 (1 - cos-theta)
  //    T = I cos-theta + S sin-theta + P   (1 - cos-theta)
  // where
  //    I is identity
  //    S is the skew-symmetric cross-product matrix representing (n-hat cross)
  //    P is the outer-product matrix (n-hat * n-hat')
  // Note that P = I + S^2 (see documentation)
  //
  // We will use the latter form, since the computation of P is faster than
  //     I + S^2
  //
  // The unit-vector basis is the unit-vector component of motor-tolerance
  // For small angles, the magnitude of that rotation is approximately equal to
  // the magnitude of the motor-tolerance vector.

  // Apply the trig functions to this magnitude once up-front
  double cos_disp = std::cos(tolerance_mag);
  double sin_disp = std::sin(tolerance_mag);

  // Now normalize the motor frame dispersion vector to get the unit vector that
  // will be used in Rodrigues' formula.
  double tolerance_unit[3];
  jeod::Vector3::scale(motor_tolerance, (1/tolerance_mag), tolerance_unit);

  // Start construction of the transformation matrix by generating the
  // Outer Product of the normalized Frame Tolerance dispersion vector
  // and scaling that by (1 - cos theta).
  jeod::Matrix3x3::outer_product( tolerance_unit,
                            tolerance_unit,
                            T_dispersion_to_nominal);
  jeod::Matrix3x3::scale((1.0 - cos_disp), T_dispersion_to_nominal);

  // Add the (I cos-theta) term:
  T_dispersion_to_nominal[0][0] +=  cos_disp;
  T_dispersion_to_nominal[1][1] +=  cos_disp;
  T_dispersion_to_nominal[2][2] +=  cos_disp;

  // Generate the skew symmetric cross-product matrix and scale by sin-theta
  double scratch[3][3];
  jeod::Matrix3x3::cross_matrix( tolerance_unit,
                           scratch);
  jeod::Matrix3x3::scale( sin_disp,
                    scratch);

  // Add this scaled skew-symmetric matrix to the previous matrix (sum of
  // outer-product and cosine-diagonal matrices).
  jeod::Matrix3x3::incr( scratch,
                   T_dispersion_to_nominal);

  // T_dispersion_to_nominal now represents the complete transformation from
  // the actual (dispersed) attitude to the nominal attitude.
  // Now, the transformation from the parent frame to the dispersed frame
  // is the product:  [T_{nominal->disp}] [T_{parent->nominal}]
  //                = [T_{disp->nominal}]' [T_{struc->nominal}]
  //
  // Because the matrix T_parent_to_motor is used as both input and output, we
  // need a copy of it so that it does not overwrite itself during the matrix
  // multiplication operation.
  jeod::Matrix3x3::copy(  T_parent_to_motor,
                    scratch);
  jeod::Matrix3x3::product_left_transpose( T_dispersion_to_nominal,
                                     scratch,
                                     T_parent_to_motor);
}