/*******************************************************************************
PURPOSE:
  (Spherical interpolation algorithm for handling quaternion interpolation.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (November 2017) (New implementation))
  )
*******************************************************************************/

#include <cmath>
#include "../include/quaternion_spherical_interpolator.hh"

#include "jeod/models/utils/math/include/vector3.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
QuaternionSphericalInterpolator::QuaternionSphericalInterpolator()
  :
  use_spherical_interp(true),
  quat_0(),
  quat_1(),
  frac(0.0),
  quat_out(),
  epsilon_interp(1.0E-15),
  epsilon_delta(1.0E-15)
{}

/*****************************************************************************
update
Purpose:(main method)
Limitation:
  If using linear_interpolation, the quaternions in the data table
  must have the same sign.
*****************************************************************************/
void
QuaternionSphericalInterpolator::update()
{
  double A = 1-frac;
  double B = frac;
  // if user has turned off the spherical interpolation, or if
  // near either end of the domain, use linear interpolation, with the A
  // and B as already computed.
  // Otherwise, generate a new A and B for spherical interpolation:

  if (use_spherical_interp &&
      frac > epsilon_interp &&
      frac < 1-epsilon_interp) {

    /* Ensure that both quaternions are normalized.
     * Avoid normalizing a zero-quaternion, jeod::Quaternion::normalize() will return
     * NaN values if the input quaternion is 0.
     * The threshold is rather arbitrary, these should be normalized
     * quaternions so the norm_sq value should be close to 1. There will only
     * be an error if it is equal to 0, triggering as an error if norm_sq is
     * less than some value between 0 and 1 is reasonable.*/
    bool quat_0_error = false;
    if (quat_0.norm_sq() < epsilon_delta) {
      quat_0_error = true;
      quat_0.set_to_zero();
    } else {
      quat_0.normalize();
    }

    if (quat_1.norm_sq() < epsilon_delta) {
      quat_1.set_to_zero();
      if (quat_0_error) { // Quat_0 and Quat_1 are both invalid
        CMLMessage::error( __FILE__, __LINE__,
            "Error in Spherical interpolation. Quat_0 and Quat_1 are invalid.\n"
            "Zeroing Quat_0 and Quat_1.\nReturning Identity.\n");
        quat_out.make_identity();
        return;

      } else { // Quat_1 is invalid, Quat_0 is good.
        CMLMessage::error( __FILE__, __LINE__,
            "Error in Spherical interpolation. Quat_1 is invalid.\n"
            "Zeroing Quat_1.\nReturning normalized Quat_0.");
        quat_out.copy_from(quat_0);
        return;
      }
    }

    else if (quat_0_error) { // Quat_0 is invalid, Quat_1 is good.
      CMLMessage::error( __FILE__, __LINE__,
          "Error in Spherical interpolation. Quat_0 is invalid.\n"
          "Zeroing Quat_0.\nReturning normalized Quat_1.");
      quat_1.normalize();
      quat_out.copy_from(quat_1);
      return;
    }

    else { // Both good , keep going.
      quat_1.normalize();
    }

    // obtain the scalar component of the quaternion covering the full
    // difference between quat_0 and quat_1.
    double q_delta_s = quat_0.scalar * quat_1.scalar +
                       jeod::Vector3::dot( quat_0.vector, quat_1.vector);


    // If the start and end of the interval are really close, q_delta_s
    // will have a value really close to 1.  In such proximity,
    //   - the difference between spherical and linear interpolation is
    //     negligible, so can perform a linear interpolation instead.
    //   - sin_delta can be small
    // Additionally, for q_delta_s > 1, delta = acos (q_delta_s) is illegal.
    // Can protect against all 3 of these by continuing with spherical
    // interpolation only if q_delta_s is not close to 1.
    if ( std::abs(q_delta_s) < 1-epsilon_delta) {

      // Because Q and -Q are equivalent for the purposes of transformation
      // quaternions, it is important to make sure that both quaternions have
      // the same sign.  Otherwise, bad results will follow when they are
      // summed.  If they are opposite sign, flip the sign on 1 of them.
      // quat_1 has been chosen arbitrarily for the sign-flip.
      // NOTE: because quat_0 and quat_1 have been normalized, q_delta_s
      // should ALWAYS be >= 0, but we leave this check in place as a safety
      // net.
      if (q_delta_s < 0.0) {
        quat_1.scalar = -quat_1.scalar;
        jeod::Vector3::negate(quat_1.vector);
        q_delta_s = -q_delta_s;
      }

      // At this point, q_delta_s in range [0, 1-epsilon_delta)
      double delta = std::acos(q_delta_s);
      double sin_delta = std::sin(delta);
      // (Note: if we only needed sin_delta, that could be obtained from
      // sqrt(1-q_delta_s^2).  But we also need delta.

      // It is not necessary to provide additional protect against sin_delta
      // being small.  In order for delta to be so small that dividing
      // by sin(delta) would incur problems:
      //     sin_delta ~= delta
      // But we already know:
      //     cos_delta ~= (1 - delta^2 / 2) < 1 - epsilon
      // Hence:
      //     delta > sqrt( 2 * epsilon) >> epsilon
      // Hence, protecting q_delta_s < 1-epsilon protects sin_delta not small
      A = std::sin( A * delta) / sin_delta;
      B = std::sin( B * delta) / sin_delta;
    }
  }
  // Now interpolate using either the linear A and B or the spherical A and
  // B if those have been computed.
  quat_out.scalar = A * quat_0.scalar + B * quat_1.scalar;
  for (unsigned int ii = 0; ii< 3; ++ii) {
    quat_out.vector[ii] = A * quat_0.vector[ii] + B * quat_1.vector[ii];
  }

  /* Note - if using SLERP interpolation, quat_0 and quat_1 have already been
   * tested and confirmed to both be non-zero so quat_out should also be
   * non-zero.
   * However, if using direct-aassignment or linear interpolation those checks
   * have not been made and quat_out could be 0 at this point.
   * Normalization of a zero quaternion is insuffciently protected and will
   * result in FE-exception NaN.
   * So we do need to check, and avoid normalizing if quat_out is 0.*/
  if (quat_out.norm_sq() < epsilon_delta) {
    CMLMessage::error( __FILE__, __LINE__,
      "Error in Spherical interpolation. Output quaternion interpolated to 0.\n"
      "Returning identity quaternion instead.\n");
    quat_out.make_identity();
  } else {
    quat_out.normalize();
  }
}

/*****************************************************************************
setters
Purpose:(set epsilon values for fine-tuning.)
*****************************************************************************/
void
QuaternionSphericalInterpolator::set_epsilon_interp( double new_epsilon)
{
  epsilon_interp = new_epsilon;
}
/****************************************************************************/
void
QuaternionSphericalInterpolator::set_epsilon_delta( double new_epsilon)
{
  if (new_epsilon < 1.0E-15) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid optional argument.\n",
      "The epsilon value has a minimum of 1E-15."
      "Resetting to minimum.\n");
    epsilon_delta = 1.0E-15;
  }
  else {
    epsilon_delta = new_epsilon;
  }
}
