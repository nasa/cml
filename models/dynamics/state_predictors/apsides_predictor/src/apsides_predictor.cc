/********************************* TRICK HEADER *******************************
PURPOSE:
   (Computation of time to apsides and altitude of apsides with J2 correction.)
REFERENCES:
  ((CEV model orb_elem_apge_prge.c at cev/models/environment/orb_elem)
   (Ascent Guidance FSSR STS 83-0002-30-A section 4.7.8))
ASSUMPTIONS AND LIMITATIONS:
PROGRAMMERS:
    ((Gary Turner) (OSR) (Oct 2014) (Antares))
*******************************************************************************/

#include <cmath> // sqrt
#include "jeod/models/environment/planet/include/planet.hh"

#include "../include/apsides_predictor.hh"


/*****************************************************************************
Constructor
Purpose:()
*****************************************************************************/
ApsidesPredictor::ApsidesPredictor(
     const jeod::RefFrameTrans & inrtl_state,
     const jeod::Planet & planet,
     const double & grav_mu)
  :
  small_tolerance(1.0e-13),
  min_del_h(18520.0), // 10 Nautical miles, per requirements
  time_to_next_apsis(0.0),
  next_apsis_type(UNDEFINED),
  apoapsis_altitude(0.0),
  periapsis_altitude(0.0),
  orb_elem( inrtl_state,
            planet,
            grav_mu),
  equatorial_radius( planet.r_eq)
{
  subscribe_name = "ApsidesPredictor:";
}

/*****************************************************************************
initialize
Purpose:(runs initialize on the OrbElemSubset model;
         no initialization needed of unique model aspects.)
*****************************************************************************/
void
ApsidesPredictor::initialize()
{
  if (!enabled) {
    return;
  }

  orb_elem.initialize();
  SubscriptionBase::initialize();
}

/*************************************************************************
Method: orbital_apsis_time_calc
Purpose: Compute orbital apogee/perigee altitude above a spherical planet
and the time to next apsis based on an input state
Reference: STS 83-0002-34 Shuttle Flight Software Subsystem Requirements (ORB ALT TIME TASK)
*************************************************************************/
void
ApsidesPredictor::update()
{
  if (!active) {
    return;
  }

  // Generate regular orbital elements
  orb_elem.update();

  /* check to see if hyperbolic or parabolic orbit */
  if (orb_elem.orbit_type != OrbElemSubset::ELLIPSE) {
    if (orb_elem.orbit_type == OrbElemSubset::HYPERBOLA) { 
      next_apsis_type = HYPERBOLA_NO_APSIS;
    } 
    else if (orb_elem.orbit_type == OrbElemSubset::PARABOLA) {
      next_apsis_type = PARABOLA_NO_APSIS;
    }
    // TODO: Compute the following if vehicle is approaching planet during fly-by 
    time_to_next_apsis = 0.0;
    apoapsis_altitude = 0.0;
    periapsis_altitude = 0.0;
    return;
  }


  /* j2 correction parameters */
  orb_elem.correct_for_j2();

  /* compute the adjusted eccentricity */
  orb_elem.compute_eccentricities();

  // check the difference in apoapsis and periapsis radii to see whether
  // orbit is near circular.
  if (2.0 * orb_elem.semi_major_axis * orb_elem.ecc_mag < min_del_h){
    next_apsis_type = CIRCULAR_NO_APSIS;
    time_to_next_apsis = 0.0;
  }
  else {
    compute_next_apsis();
  }

  /* compute apoapsis and periapsis altitude above a spherical planet */
  apoapsis_altitude = orb_elem.semi_major_axis * (1.0 + orb_elem.ecc_mag) +
                      orb_elem.j2_delta - equatorial_radius;
  periapsis_altitude= orb_elem.semi_major_axis * (1.0 - orb_elem.ecc_mag) +
                      orb_elem.j2_delta - equatorial_radius;
}

/*****************************************************************************
compute_next_apsis
Purpose:(Continues the process in the event that orbit is elliptical)
*****************************************************************************/
void
ApsidesPredictor::compute_next_apsis()
{
  // Compute the time to next apsis, sine of true anomaly,
  // cosine of true anomaly
  double sign_rdot = 1.0; // used later, do not delete.
  if (orb_elem.vel_vertical_scalar >= 0.0) {
    next_apsis_type = APOAPSIS_NEXT_APSIS;
    time_to_next_apsis = M_PI; // temporary
  }
  else {
    sign_rdot = -1.0;
    next_apsis_type = PERIAPSIS_NEXT_APSIS;
    time_to_next_apsis = 0.0; // temporary
  }

  // FIXME G. Turner Oct 2014.  This is from the original algorithm, but it
  // uses a mean motion that is computed after the first semi-major-axis
  // correction with a mean anomaly which is based off the second.
  // Recommend independent verification of the original algorithm.

  // The mean anomaly required to target = (
  //      target mean-anomaly (just set) - current mean-anomaly
  // time to target = mean-anomaly t- target / mean motion.
  time_to_next_apsis -= (orb_elem.ecc_anomaly - orb_elem.e_sinE);
  time_to_next_apsis /= orb_elem.mean_motion;

  /* compute a correction term to correct time to next apsis */
  // NOTE:   This semi-major-axis and eccentricity are
  // twice-corrected, but the semi-parameter is uncorrected.
  // An alternative formulation is:
  // double KK = orb_elem.semi_major_axis * orb_elem.ecc_mag /
  //             (4.0 * (1 - orb_elem.ecc_mag * orb_elem.ecc_mag));
  // but the differences are minimal.
  double KK = (orb_elem.semi_major_axis * orb_elem.semi_major_axis *
               orb_elem.ecc_mag) / (4.0 * orb_elem.semi_parameter);


  /* check to ensure correction term is in an allowable range */
  if (KK <= orb_elem.j2_correction * orb_elem.sin_sq_I){
    return;
  }

  // NOTE - do not use compute_true_anomaly().  The semi-parameter has not been
  // adjusted and the position_mag and ecc_mag may be inconsistent.
  double cosNu = (orb_elem.e_cosE - orb_elem.ecc_mag * orb_elem.ecc_mag) /
                 (orb_elem.ecc_mag * (1.0 - orb_elem.e_cosE) );
  // Add some protection that the values are legal.
  if (cosNu > 1.0) {
    cosNu = 1.0;
  } else if (cosNu < -1.0) {
    cosNu = -1.0;
  }
  double sinNu = std::sqrt(1 - cosNu * cosNu) * sign_rdot;

  /* recompute correction factor to include true anomaly */
  // ALGEBRA NOTE: Simplifies to
  // orb_elem.j2_delta = K (sin^2 (incl)) cos( 2* arg_periapsis)
  orb_elem.j2_delta *= (cosNu*cosNu - sinNu*sinNu);
  orb_elem.j2_delta += (4.0 * orb_elem.j2_correction * sinNu * cosNu *
                        orb_elem.sinI_sinL * orb_elem.sinI_cosL);


  // compute a quantity proportional to the sine of twice the
  // argument of perigee
  // ALGEBRA NOTE: Simplifies to
  // double sin_2arg_peri = K * (sin^2 (incl)) * sin(2 * arg_periaps)
  double sin_2arg_peri = 2.0 * orb_elem.j2_correction *
            (sinNu * cosNu * (orb_elem.sinI_sinL * orb_elem.sinI_sinL -
                              orb_elem.sinI_cosL * orb_elem.sinI_cosL) -
             orb_elem.sinI_sinL * orb_elem.sinI_cosL * (sinNu * sinNu -
                                                        cosNu * cosNu) );

  // For non-zero value of sin_2arg_peri compute time correction term and
  // time to next apsis.  Otherwise, finished.
  if ((sin_2arg_peri < small_tolerance) &&
      (sin_2arg_peri > -small_tolerance)){
    return;
  }

  KK = -(sign_rdot * KK + orb_elem.j2_delta) / sin_2arg_peri;
  // KK now dimensionless
  if (KK < 0.0 ) {
    KK -= std::sqrt(KK * KK + 2.0);
  } else {
    KK += std::sqrt(KK * KK + 2.0);
  }
  // FIXME G. Turner Oct 2014
  // mean motion is once-corrected, ecc_mag is twice-corrected.
  // Effect is minimal.
  time_to_next_apsis += ((1.0 + 2.0 * sign_rdot * orb_elem.ecc_mag) /
                         (KK * orb_elem.mean_motion));

  // if time to next apsis is negative, add half the period and
  // flip the type
  if (time_to_next_apsis < 0.0){
    time_to_next_apsis = time_to_next_apsis + M_PI / orb_elem.mean_motion;
    /* switch next apsis */
    if (next_apsis_type == APOAPSIS_NEXT_APSIS) {
      next_apsis_type = PERIAPSIS_NEXT_APSIS;
    }
    else if (next_apsis_type == PERIAPSIS_NEXT_APSIS) {
      next_apsis_type = APOAPSIS_NEXT_APSIS;
    }
  }
}

/*****************************************************************************
activate
Purpose:(Switches the model on)
*****************************************************************************/
void
ApsidesPredictor::activate()
{
  active = true;
  update();
}
