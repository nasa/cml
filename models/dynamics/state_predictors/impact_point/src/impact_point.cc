/********************************* TRICK HEADER *******************************
PURPOSE:
   (Parameter definition for ballistic impact predictor function.)

REFERENCES:
    ((JEOD Orbital Elements model)
     ((Vallado, D.) (Fundamentals of Astrodynamics and Applications)))

PROGRAMMERS:
   (((Gary Turner) (OSR) (Oct 2014) (Antares))
    ((Bingquan Wang) (OSR) (May 2017) (Antares) 
                     (added the null pointer check per the IV&V code review))
   )
*******************************************************************************/

#include <cmath>   // sin, cos
#include <cstdlib> // abs
#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/ref_frames/include/ref_frame_state.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/impact_point.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
ImpactPoint::ImpactPoint(const double & time,
                         const jeod::RefFrameTrans & inrtl_state,
                         const jeod::Planet & planet_in,
                         const double & grav_mu_in)
  :
  dyn_time(time),
  planet_ref(planet_in),
  equatorial_radius(planet_ref.r_eq),
  grav_mu( grav_mu_in),
  auto_off(true),
  iteration_threshold(1.0),
  iteration_count_max(20),
  surface_range(0.0),
  time_to_impact(0.0),
  time_of_impact(0.0),
  impact_type(UNDEFINED),
  launch_range(*this),
  planet_ecc_factor(0.0),
  orb_elem(inrtl_state,
           planet_ref,
           grav_mu)
{
  subscribe_name = "ImpactPoint:";
  launch_range.subscribe();
}

/*****************************************************************************
initialize
Purpose:(Initializes the model.  
         Sets the launch-range calculation references, interpreting the:
         reference position = input argument in pfix
         reference direction = pfix, z-axis.
         Direction is arbitrary but is necessary for initialization.)
*****************************************************************************/
void
ImpactPoint::initialize(
    double reference_position_pfix[3])
{
  if (!reference_position_pfix) {
    CMLMessage::fail(__FILE__, __LINE__, "Null pointer\n", 
                         "The input pointer 'reference_position_pfix' is null.");  
  }
  // Set included model data
  launch_range.reference_data.position_type = PfixReferencePoint::pfix;
  jeod::Vector3::copy(reference_position_pfix,
                launch_range.reference_data.position);
  launch_range.reference_data.direction_type =  PfixReferencePoint::pfix;
  jeod::Vector3::unit(2, launch_range.reference_data.direction);
  
  initialize( );
}

/*****************************************************************************
initialize
Purpose:(Sets the pointers to the external data)
Assumptions:(The launch_range reference data has already been set.
             If not, use the initialize method with the additional argument.)
*****************************************************************************/
void
ImpactPoint::initialize()
{
  if (!enabled) {
    return;
  }

  // initialize up the tree
  // Note - need the const-cast here because PlanetFixedPosition::initialize
  //        does not have a const in the argument even though the pointer is
  //        treated as const.
  jeod::PlanetFixedPosition::initialize(const_cast<jeod::Planet *>(&planet_ref));

  // Compute model variables
  planet_ecc_factor = planet_ref.e_ellip_sq / ( 1.0 - planet_ref.e_ellip_sq);

  // initialize included models
  orb_elem.initialize( );
  launch_range.initialize(planet_ref);
  launch_range.set_reference_data();
  SubscriptionBase::initialize();
}


/*****************************************************************************
update
Purpose:(The main update method)
*****************************************************************************/
void
ImpactPoint::update()
{
  /* Check whether model is active.  If not, return. */
  if (!active) {
    return;
  }

  // Compute (sub)-orbital elements
  orb_elem.update();
  orb_elem.compute_eccentricities();

  if (orb_elem.orbit_type != OrbElemSubset::ELLIPSE)  {
   // NO IMPACT POINT:  Either parabolic or hyperbolic orbit
   // Set ballistic impact point parameters to large negative value (-9999).
   // Latitude and longitude are logged in degrees, so need to convert from
   // radians to degrees for the log files to show -9999.
   ellip_coords.latitude =
   ellip_coords.longitude = -174.5154719;
   surface_range =
   time_to_impact =
   time_of_impact = -9999.;
   if (orb_elem.orbit_type == OrbElemSubset::HYPERBOLA) {
     impact_type = HYPERBOLA_NO_IMPACT;
   }
   else if (orb_elem.orbit_type == OrbElemSubset::PARABOLA) {
     impact_type = PARABOLA_NO_IMPACT;
   }
   return;
  }

  if ( orb_elem.semi_major_axis * (1 - orb_elem.ecc_mag) > equatorial_radius){
     // No possibility of impact.  Orbit too high
     // Set ballistic impact point parameters to zero.
     ellip_coords.latitude =
     ellip_coords.longitude =
     surface_range =
     time_to_impact =
     time_of_impact = 0.0;
     impact_type = HIGH_ORBIT_NO_IMPACT;
     return;
  }

  double sin_sq_latitude = orb_elem.position_unit[2];
  sin_sq_latitude *= sin_sq_latitude; // square of sin(latitude)

  double planet_surface_radius = equatorial_radius /
                    std::sqrt ( 1.0 + planet_ecc_factor * sin_sq_latitude );

  if (orb_elem.position_mag < planet_surface_radius) {
    // ACTUAL VEHICLE IMPACT HAS EITHER OCCURRED OR VEHICLE IS
    // STARTING BELOW EARTH MODEL SURFACE:  Set ballistic impact
    // point parameters equal to current conditions.
    jeod::Vector3::transform(planet_ref.pfix.state.rot.T_parent_this,
                       orb_elem.position,
                       cart_coords);
    update_from_cart(cart_coords);
    ellip_coords.altitude = 0.0;
    update_from_ellip(ellip_coords);
    launch_range.update();
    surface_range = launch_range.total_range_avg_rad;
    time_to_impact = 0.0;
    time_of_impact = dyn_time;
    impact_type = BELOW_SURF_POST_IMPACT;
    if (auto_off) {
      SubscriptionBase::deactivate();
    }
    return;
  }

  // Obtain the true-anomaly for the current position
  orb_elem.compute_true_anomaly();

  // For equatorial orbits, use a different method.
  if (orb_elem.sin_sq_I < 1.0E-12) {
    update_equatorial();
  }
  else {
    update_non_equatorial();
  }

  // Compute the time to impact
  update_time();

  // Compute the impact range.
  launch_range.update();
  surface_range = launch_range.total_range_avg_rad;
  impact_type = IMPACT_PENDING;
}

/*****************************************************************************
update_equatorial
Purpose:(Continues the update process in the case of an equatorial orbit)
*****************************************************************************/
void
ImpactPoint::update_equatorial()
{
  // Transform into pfix coordinates
  jeod::Vector3::transform( planet_ref.pfix.state.rot.T_parent_this,
                      orb_elem.position,
                      cart_coords);
  // Transform into geodetic and geocentric coordinates
  update_from_cart(cart_coords);
  // Store off the true-anomaly for the current position
  double this_nu = orb_elem.true_anomaly;

  // Solve true-anomaly at equatorial radius
  orb_elem.position_mag = equatorial_radius;
  orb_elem.vel_vertical_scalar = -1.0;
  orb_elem.compute_true_anomaly();

  // longitude advances with true-anomaly for an equatorial orbit, so
  // (impact long. - current long.) = (impact anomaly - current anomaly)
  // Generate impact longitude (and altitude):
  sphere_coords.longitude += (orb_elem.true_anomaly - this_nu);
  sphere_coords.altitude = 0.0;

  // Transform into Cartesian and geodetic coordinates
  update_from_spher(sphere_coords);  
}


/*****************************************************************************
update_non_equatorial
Purpose:(Continues the update process in the general non-equatorial case)
*****************************************************************************/
void
ImpactPoint::update_non_equatorial()
{
  // Obtain the argument of periapsis for the general orbit.
  orb_elem.compute_arg_periapsis();

  // Initial guess.  Compute the true anomaly that corresponds to the
  // descending position  equal to the equatorial radius.
  unsigned int iteration_count = 0;
  bool convergence = false; // switch to true on convergence
  double planet_surface_radius = equatorial_radius;

  // Should be finished with this; ensure it is negative so that we get the
  // descending position.
  orb_elem.vel_vertical_scalar = -1.0;

  while (!convergence && (iteration_count < iteration_count_max)) {
    iteration_count++;

    // With this call, the position_mag and true_anomaly represent the values
    // at the test point, not at the vehicle any more.  orb_elem.position is
    // still the position of the the vehicle.  DO NOT CONFUSE THESE.
    orb_elem.position_mag = planet_surface_radius;
    orb_elem.compute_true_anomaly();

    // Compute the square of the sine of the angle from the ascending node to
    // the current estimated impact point.
    double sin_sq_node_angle = std::sin(
                                  orb_elem.true_anomaly + orb_elem.arg_periapsis);
    sin_sq_node_angle *= sin_sq_node_angle;

    // Compute the square of the sine of the latitude.  Sign is not important,
    // symmetry about equator.  Similarly, sign of node-angle not important.
    double sin_sq_latitude = sin_sq_node_angle * orb_elem.sin_sq_I;

    // Compute the surface radius
    planet_surface_radius  = equatorial_radius /
                      std::sqrt ( 1.0 + planet_ecc_factor * sin_sq_latitude );

    double position_error = std::abs(orb_elem.position_mag - planet_surface_radius);
    if (position_error < iteration_threshold) {
      convergence = true;
    }
  }

  // If failed to reach convergence:
  if (!convergence) {
    ellip_coords.latitude =
    ellip_coords.longitude = 174.5154719; //So the log files (degrees) show 9999
    surface_range =
    time_to_impact =
    time_of_impact =   9999.;
    return;
  }

  // Now have a location on the descending path where the orbital radius is
  // equal to the surface radius.  Still need to convert this to a position.

  // This call will overwrite orb_elem.position with the inertial position of
  // the impact point.
  orb_elem.generate_cartesian_inertial();

  // Transform into pfix coordinates
  jeod::Vector3::transform( planet_ref.pfix.state.rot.T_parent_this,
                      orb_elem.position,
                      cart_coords);
  // Transform into geodetic and geocentric coordinates
  update_from_cart(cart_coords);
}


/*****************************************************************************
update_time
Purpose:(Compute the time to impact)
*****************************************************************************/
void
ImpactPoint::update_time()
{
  // Now the time:  Need the eccentric anomaly
  //  NOTE - DO NOT use compute_eccentricities().  That requires velocity,
  //  which we do not have and do not need.
  double e_cosE_impact =
                (1.0 - orb_elem.position_mag / orb_elem.semi_major_axis);
  double cosE_impact = e_cosE_impact / orb_elem.ecc_mag;
  // sinE should be negative (return side of orbit)
  double e_sinE_impact = -orb_elem.ecc_mag *
                                  std::sqrt( 1.0 - cosE_impact * cosE_impact);
  double E_impact = std::atan2( e_sinE_impact, e_cosE_impact);

  time_to_impact = orb_elem.semi_major_axis * std::sqrt(orb_elem.semi_major_axis /
                                                        grav_mu) *
                   ((E_impact - orb_elem.ecc_anomaly) -
                    (e_sinE_impact - orb_elem.e_sinE));
  time_of_impact = dyn_time + time_to_impact;

  // Adjust the impact longitude to account for the rotation of the planet
  // during the time-to-impact
  sphere_coords.longitude -=
               planet_ref.pfix.state.rot.ang_vel_this[2] * time_to_impact;
  update_from_spher(sphere_coords);
  // NOTE - do not need to re-iterate; the planetary radius does not change
  // with longitude.
}


/*****************************************************************************
activate
Purpose:(Switches the model on)
*****************************************************************************/
void
ImpactPoint::activate()
{
  active = true;
  update();
}
