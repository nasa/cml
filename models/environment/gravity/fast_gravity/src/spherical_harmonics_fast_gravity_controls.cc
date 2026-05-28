/*******************************************************************************
Purpose:
  (Define member functions for the SphericalHarmonicsFastGravityControls class.)

Programmers:
 (((Gary Turner) (OSR) (May 2014) (Antares) (Implementation of Blair Thompson's
                     algorithm originally written for earlier JEOD))
  ((Bignquan Wang) (OSR) (April 2017) (Antares) (Fixed the compilation warning of
                     float-point number equality comparision))
  ((Bingquan Wang) (OSR) (June 2017) (Antares) (Code cleanup for IV&V))
 )

*******************************************************************************/

#include <cmath>
#include <limits>
#include "jeod/models/environment/gravity/include/spherical_harmonics_gravity_source.hh"
#include "jeod/models/environment/planet/include/planet.hh"
#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"

#include "../include/spherical_harmonics_fast_gravity_controls.hh"

/*******************************************************************************
  Function: SphericalHarmonicsFastGravityControls::
                                          SphericalHarmonicsFastGravityControls
  Purpose:  (Default constructor)
*******************************************************************************/
SphericalHarmonicsFastGravityControls::SphericalHarmonicsFastGravityControls()
 :
   fast_enabled(true),
   count_limit(10),
   compute_count_limit(true),
   threshold_ratio_delta_acc(1.0E-7),
   available(false),
   first_pass(true),
   count(0),
   reference_potential(0.0)
{
   jeod::Vector3::initialize(reference_pos_pfix);
   jeod::Vector3::initialize(delta_pos);
   jeod::Vector3::initialize(reference_accel_pfix);
   jeod::Matrix3x3::initialize(reference_gradient_pfix);
   jeod::Matrix3x3::initialize(reference_gradient_inrtl);
}


/*******************************************************************************
  Function: SphericalHarmonicsFastGravityControls::initialize
  Purpose:  (Sets the encessary parameters)
  Class:    (initialization)
*******************************************************************************/
void
SphericalHarmonicsFastGravityControls::initialize_control ( // Return: --   Void
    jeod::GravityManager &grav_manager)          // In:     -- Ref to gravity manager
{
  jeod::SphericalHarmonicsGravityControls::initialize_control(grav_manager);

  if (fast_enabled) {
    gradient = true;  // Must compute gradient.
  }

  // pfix is also necessary.  That gets subscribed by the method
  // GravityControls::reset_controls, which is called as an element
  // ultimately of jeod::DynManager::initialize_simulation.
  // This class gets the same behavior by inheritance.
}


/*******************************************************************************
  Function: SphericalHarmonicsFastGravityControls::calc_nonspherical
  Purpose:  (Compute the gravitation at a given position toward a gravity body.)
  Class:    (derivative)
*******************************************************************************/
void
SphericalHarmonicsFastGravityControls::calc_nonspherical(// Return: --   Void
   const double integ_pos[3], // In: unused
   const double inertial_position[3], // In: M Point of interest, inrtl frm
   const jeod::GravityIntegFrame & grav_source_frame, // In: Unused
   double body_grav_accel[3], // Out: M/s2 Accel for given grav body
   double dgdx[3][3],         // Out: 1/s2 Gradient for given grav body
   double & pot)              // Out: --   Potential
{
  if (!fast_enabled) {
    jeod::SphericalHarmonicsGravityControls::calc_nonspherical(
                                   integ_pos,
                                   inertial_position,
                                   grav_source_frame,
                                   body_grav_accel,
                                   dgdx,
                                   pot);

    available = false;  // Must restart if it gets re-enabled.
    first_pass = true;
    return;
  }

  // Enabled.
  // WARNING: the IntegrationControls method produces a dgdx that is
  //          expressed relative to the inertial frame.  For maximum
  //          longevity of the validity of the approximation, it would
  //          be better if this was done in pfix.  Indeed, store all of
  //          the reference values in pfix.

  // Store the transformation matrix rather than keep accessing it
  // through pointers.
  double T_inrtl_to_pfix[3][3] = {{0x00}};
  jeod::Matrix3x3::copy(harmonics_source->pfix->state.rot.T_parent_this,
                  T_inrtl_to_pfix);

  // Compute position of vehicle wrt the planet center.
  double pfix_position[3] = {0x00};

  jeod::Vector3::transform (T_inrtl_to_pfix,
                      inertial_position,
                      pfix_position);


  // Increment the count of the number of computations made since the last
  // full update
  ++count;
  if (count >= count_limit) {
    available = false;
  }

  // NOTE - continue to  perform fast gravity even if flagged as
  // unavailable if the count limit is computed so that they may be
  // compared.
  double accel_scratch[3] = {0x00};
  if (available || compute_count_limit) {
    // Perform the fast gravity computation.
    jeod::Vector3::diff(pfix_position,
                  reference_pos_pfix,
                  delta_pos);

    jeod::Vector3::transform(reference_gradient_pfix,
                       delta_pos,
                       accel_scratch); // delta-acceleration
    jeod::Vector3::incr(reference_accel_pfix,
                  accel_scratch); // total acceleration.
  }

  if (available){
    // NOTE the variable dyn_body.grav_interaction.grav_pot, to which pot
    // references, is actually the negative gravitational potential.  So while
    // grav potential will decrease by the dot product, the variable will
    // increase by the same amount.
    pot = reference_potential + jeod::Vector3::dot(reference_accel_pfix,
                                               delta_pos);
    jeod::Vector3::transform_transpose(T_inrtl_to_pfix,
                                 accel_scratch,
                                 body_grav_accel); // in inertial.
    // End of the routine, be sure to populate the dgdx matrix.
    jeod::Matrix3x3::copy(reference_gradient_inrtl,
                    dgdx);
    return;
  }

  // Else, the fast method has been flagged as not available (but is still
  // enabled).  In this case, we need to run a comparison between the
  // currently held gradient and a new full computation of the gradient.
  // Note - if we need to do a comparison, "accel_scratch" currently holds
  // the pfix reference frame gravity vector.
  jeod::SphericalHarmonicsGravityControls::calc_nonspherical (
                                 integ_pos,
                                 inertial_position,
                                 grav_source_frame,
                                 body_grav_accel,
                                 dgdx,
                                 pot);

  // Record the parameters associated with this most recent full-harmonic
  // computation:

  // The position relative to the gravity body must be recomputed, it
  // was not stored off in the GravityControls::gravitation call.
  jeod::Vector3::copy(pfix_position,
                reference_pos_pfix);

  // zero the delta-position
  jeod::Vector3::initialize(delta_pos);

  // Transform the computed grav acceleration to pfix and store it.
  jeod::Vector3::transform (T_inrtl_to_pfix,
                      body_grav_accel,
                      reference_accel_pfix);

  // Transform the dgdx array to pfix and store it.
  // dgdx can be transformed by the application [T][dg/dx][T]'
  // with T being the transformation from inertial to pfix.
  jeod::Matrix3x3::transform_matrix (T_inrtl_to_pfix,
                               dgdx,
                               reference_gradient_pfix);

  // Make a copy of the inertially-referenced dgdx array
  jeod::Matrix3x3::copy(dgdx,
                  reference_gradient_inrtl);

  // Copy the potential; potential is independent of frame.
  reference_potential = pot;

  // Make fast gravity available again, and zero the count since the last
  // full-computation.
  available = true;
  count = 0;

  // if this is the first time through, the fast gravity data is junk, we
  // do not want to compare it against computation. So we are done.
  if (first_pass) {
    first_pass = false;
    return;
  }

  // If the count-limit is pre-set, then there is no point in comparing the
  // errors because we are not going to do anything about it anyway.
  if (!compute_count_limit) {
    return;
  }

  // Else, the count-limit is variable and we need to compare the error
  // induced by the most recent count limit against the allowable threshold
  // for possible realignment of the count-limit.

  // If the allowable threshold is very small, then by default recompute the
  // full field when count = 1 (i.e. every cycle)
  if (std::fabs(threshold_ratio_delta_acc) <
                                  std::numeric_limits<double>::min()) {
    count_limit = 1;
    return;
  }

  // Otherwise, compare the error against the threshold
  // Take the difference between the fast gravity value and the full-up
  // new computation.  Recall that accel_scratch is the fast-grav-produced
  // pfix-referenced acceleration vector.  Subtract the true pfix-referenced
  // acceleration from it.
  jeod::Vector3::decr(reference_accel_pfix,
                accel_scratch);

  // Divide the (magnitude of the difference between the fast-gravity
  // computation and the full gravity computation) by the (magnitude of
  // the full gravity computation).  This gives the error as a fraction
  // of the true value.
  // Divide that value by the threshold. to give a multiple of the
  // threshold that this set of fast gravity computations iproduced

  // accel_scratch is now the delta-accel: (fast-grav accel - full-grav accel)
  double ratio_compare = (jeod::Vector3::vmag(accel_scratch) /
                          jeod::Vector3::vmag(reference_accel_pfix)) /
                          threshold_ratio_delta_acc;


  // Adjust the count_limit to maximize the step size without exceeding
  // the threshold (much).
  if (ratio_compare < 0.5) {  // count limit far too conservative
     count_limit *=2;
  }
  else if (ratio_compare > 3.0) { // count_limit far too high
     count_limit = 1;
  }
  else {
     count_limit = std::floor(count_limit / ratio_compare);
     if (count_limit < 1) {
       count_limit = 1;
     }
  }
}
