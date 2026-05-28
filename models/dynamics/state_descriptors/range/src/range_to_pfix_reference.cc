/********************************* TRICK HEADER *******************************
Purpose:
   (Completes the calculation of range for the specific case of range from a
   fixed reference location.)

Programmers:
   (((Gary Turner) (OSR) (Sept 2014) (Initial version)))

*******************************************************************************/

#include "jeod/models/environment/planet/include/planet.hh"
#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/range_to_pfix_reference.hh"

/*****************************************************************************
Constructors
*****************************************************************************/
RangeToPfixReference::RangeToPfixReference(
  const jeod::PlanetFixedPosition & state_in)
  :
  RangeComputation(state_in),
  direction(dummy_direction),
  dummy_direction()
{
  set_direction = true;
}
/****************************************************************************/
RangeToPfixReference::RangeToPfixReference(
  const jeod::PlanetFixedPosition & state_in,
  const double (&direction_)[3])
  :
  RangeComputation(state_in),
  direction(direction_),
  dummy_direction()
{
// set_direction is set to false in RangeComputation constructor
}

/*****************************************************************************
set_reference_data
Purpose:(sets the data for the reference_point and assigns it to the target)
*****************************************************************************/
void
RangeToPfixReference::set_reference_data()
{
  RangeComputation::set_reference_data();
  if (!ref_point_set) {
    return;
  }
  if (use_polar_direction) {
    target_sin_lat = ref_sin_lat;
    target_cos_lat = ref_cos_lat;
    target_longitude = reference_data.sphere_coords.longitude;

    // azimuth always measured from the origin.  This case rarely used.
    origin_sin_azimuth = std::sin(reference_data.azimuth);
    origin_cos_azimuth = std::cos(reference_data.azimuth);
  }
  else {
    jeod::Vector3::copy(reference_position_unit_pfix,
                  target_position_unit_pfix);
    // direction checked for 0 magnitude in PfixReferencePoint::set_direction()
    jeod::Vector3::copy(reference_data.direction,
                  origin_direction);
  }
}

/*****************************************************************************
update
Purpose:(updates the range values)
Assumption: (The direction vector is expressed in the frame specified in
             reference_data.direction_type (either pfix or inertial)
*****************************************************************************/
void
RangeToPfixReference::update()
{
  if (!active || !ref_point_set) {
    return;
  }

  // Only need to modify the direction if direction is not fixed (set).
  if (!set_direction) {
    /* Note - Constructors ensure that if direction is not fixed and
              pointer to direction vector is NULL, then sim is terminated.*/

    /* If direction has a magnitude of zero, the result will be the same as if
       the position and direction vectors were parallel. See the comment in
       RangeFromPfixReference::set_reference_direction().
       Also pay heed to the warning in section 5.3.3 of the documentation if
       the direction is nearly vertical.*/

    /* Copy the externally generated direction into origin_direction, and
       transform to pfix if it is in inertial.*/
    jeod::Vector3::copy (direction, origin_direction);
    if (reference_data.direction_type == PfixReferencePoint::inertial) {
      jeod::Vector3::transform( reference_data.planet->pfix.state.rot.T_parent_this,
                          origin_direction);
    }
  }

  RangeComputation::update();
}

/*****************************************************************************
update_using_polar
Purpose:(Updates the range using the polar values - longitude, latitude, and a
         direction specified by azimuth)
*****************************************************************************/
void
RangeToPfixReference::update_using_polar()
{
  // sin and cos of geocentric latitude.
  origin_sin_lat = std::sin(state.sphere_coords.latitude);
  origin_cos_lat = std::cos(state.sphere_coords.latitude);
  origin_longitude = state.sphere_coords.longitude;

  RangeComputation::update_using_polar();
}

/*****************************************************************************
update_using_cartesian
Purpose:(Updates the range using the cartesian values - a 3-vector for position)
*****************************************************************************/
void
RangeToPfixReference::update_using_cartesian()
{
  jeod::Vector3::scale(state.cart_coords,
                 (1/current_radius),
                 origin_position_unit_pfix);
  // Take the cross-product of the current position and velocity to get a
  // vector perpendicular to the plane of motion.
  jeod::Vector3::cross( origin_direction,
                  origin_position_unit_pfix,
                  dir_x_pos_unit_pfix);
                  /* NOTE -- this is [b] vector from documentation,
                  section 4.2, derivation of angle beta. */

  /* NOTE
     origin_position_unit_pfix is derived from a dynamic state, so it is
     possible (though unlikely) that it could be oriented to a direction
     parallel to origin_direction if origin_direction is near vertical.
     If origin_direction and origin_position_unit_pfix become parallel, their
     cross product (dir_x_pos_unit_pfix) will be zero.
     No warning is printed here.
     The result will be the same as if origin_direction was assigned a zero
     vector -- the cross-range angle will be zero, and the down-range angle
     will be equal to the total-range angle.
     If the two vectors are closely aligned but not parallel, the results
     of the range computation are open to misinterpretation.  See warning in
     section 5.3.3. of the documentation for details. */

  jeod::Vector3::normalize( dir_x_pos_unit_pfix);
  /* NOTE -- this is "[b]-hat" from documentation, section 4.2, derivation
             of angle beta. */

  RangeComputation::update_using_cartesian();
}
