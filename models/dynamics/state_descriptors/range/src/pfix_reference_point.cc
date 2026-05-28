/********************************* TRICK HEADER *******************************
Purpose:
   (Sets the characteristics of the fixed reference location.)

Programmers:
   (((Gary Turner) (OSR) (Sept 2014 (Initial version)))

*******************************************************************************/

#include "jeod/models/environment/planet/include/planet.hh"
#include "jeod/models/utils/math/include/vector3.hh"

#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/pfix_reference_point.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
PfixReferencePoint::PfixReferencePoint()
  :
  position_type(undefined),
  direction_type(undefined),
  latitude(0.0),
  longitude(0.0),
  altitude(0.0),
  position{0.0, 0.0, 0.0},
  azimuth(0.0),
  direction{0.0, 0.0, 0.0}
{}

/*****************************************************************************
initialize (inherit from PlanetFixedPosition)
*****************************************************************************/


/*****************************************************************************
set_point
Purpose:(sets the reference-point position based on the input values.)
Arguments:
  - reference_radius is a protected member of RangeComputation.  It is
      computed here and passed back to RangeComputation.
  - ref_point_set is also a protected member of RangeComputation, it
      identifies the success or failure of the sequential methods for setting
      the reference data.  On this, its first exposure, it will be set to
      either true or false depending on whether the reference-radius has
      been assigned a valid value. Setting this flag to false will block the
      rest of the initialization and subsequent updates from executing.
*****************************************************************************/
void
PfixReferencePoint::set_point(
          double & reference_radius,
          bool   & ref_point_set)
{

  switch (position_type) {
  case topodetic:
    ellip_coords.longitude = longitude;
    ellip_coords.latitude =  latitude;
    ellip_coords.altitude =  altitude;
    update_from_ellip( ellip_coords);
    break;
  case topocentric:
    sphere_coords.longitude = longitude;
    sphere_coords.latitude =  latitude;
    sphere_coords.altitude =  altitude;
    update_from_spher( sphere_coords);
    break;
  case inertial:
    jeod::Vector3::transform( planet->pfix.state.rot.T_parent_this,
                        position);
    // fallthrough, reference_position is now in pfix.
    [[fallthrough]];
  case pfix:
    jeod::Vector3::copy( position, cart_coords);
    update_from_cart( cart_coords);
    break;
  case undefined:
    CMLMessage::error(
      __FILE__,__LINE__,"PfixReferencePoint initialization error:\n",
      "PfixReferencePoint::position_type has not been defined prior to a call\n"
      " to set the position.\n  Insufficient information to determine how to "
      "set the position.\n");
    return;
  }

  reference_radius = jeod::Vector3::vmag(cart_coords);

  // Check the validity of the reference_radius value.
  // NOTE - This cannot fail given the current functionality in JEOD.
  // If the Cartesian position is [0, 0, 0], or the altitude such that the
  // radial distance evaluates to zero, the JEOD methods in
  // PlanetFixedPosition (e.g. cart_to_spher, cart_to_ellip) will have
  // already terminated the sim.  However, for robustness against future
  // JEOD changes, we check it here as well.
  if (reference_radius > 0.0) {
    ref_point_set = true;
  }
  else {
    ref_point_set = false;
    CMLMessage::warn(
      __FILE__,__LINE__,"PfixReferencePoint initialization error:\n",
      "The reference position was set to zero in the call to set_point(...).\n"
      "This was probably unintentional.\n");
  }
}

/*****************************************************************************
set_direction
Purpose:( sets the reference direction based on the input values)
Arguments:
  - use_polar_direction is provided for this method to set; it is a protected
      member of RangeComputation.
  - ref_point_set is also a protected member of RangeComputation, it
      identifies the success or failure of the sequential methods for setting
      the reference data.  If it has already failed, this method does nothing.
      If the initialization is still valid on entering, the method will proceed
      and flip the flag on encountering a problem.
*****************************************************************************/
void
PfixReferencePoint::set_direction(
          bool & use_polar_direction,
          bool & ref_point_set)
{
  if (!ref_point_set) {
    return;
  }

  switch (direction_type) {
  case topocentric:
  case topodetic:
    // no action, use azimuth.
    use_polar_direction = true;
    break;
  case inertial:
    jeod::Vector3::transform( planet->pfix.state.rot.T_parent_this,
                        direction);
    // fallthrough, reference_direction is now in pfix.
    [[fallthrough]];
  case pfix:
    // TODO Turner 09/2014 convert to azimuth
    //      non-critical forward work.
    use_polar_direction = false;
    if (MathUtils::is_near_equal(jeod::Vector3::vmag(direction), 0.0)) {
      ref_point_set = false;
      CMLMessage::error(__FILE__, __LINE__,
        "PfixReferencePoint initialization error:\n",
        "The reference direction was set to zero when set_direction(...).\n"
        "was called. This was probably unintentional.\n");
    }
    break;
  case undefined:
    ref_point_set = false;
    CMLMessage::error(
      __FILE__,__LINE__,"PfixReferencePoint initialization error:\n",
      "PfixReferencePoint::direction_type has not been defined prior to a call\n"
      "to set the direction.\n  Insufficient information to determine how to "
      "set the direction.\n");
    break;
  }
}
