/********************************* TRICK HEADER *******************************
Purpose:
   (Completes the calculation of range for the specific case of range from a
   fixed reference location.))

Programmers:
   (((Gary Turner) (OSR) (Sept 2014 (Initial version)))
*******************************************************************************/

#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/range_from_pfix_reference.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
RangeFromPfixReference::RangeFromPfixReference(
  const jeod::PlanetFixedPosition & state_in)
  :
  RangeComputation(state_in) // pass-through
{
  set_direction = true;
}


/*****************************************************************************
set_reference_data
Purpose:(sets the data for the reference_point and assigns it to the origin)
*****************************************************************************/
void
RangeFromPfixReference::set_reference_data()
{
  RangeComputation::set_reference_data();
  if (!ref_point_set) {
    return;
  }
  if (use_polar_direction) {
    origin_sin_lat = ref_sin_lat;
    origin_cos_lat = ref_cos_lat;
    origin_longitude = reference_data.sphere_coords.longitude;

    origin_sin_azimuth = std::sin(reference_data.azimuth);
    origin_cos_azimuth = std::cos(reference_data.azimuth);
  }
  else {
    jeod::Vector3::copy(reference_position_unit_pfix,
                  origin_position_unit_pfix);

    /* In PfixReferencePoint::set_direction() (called from
       RangeComputation::set_reference_data()), from 1st line of this
       method, reference_data.direction was:
         - forced to be expressed in planet-fixed frame, and
         - checked for 0 magnitude: if zero magnitude was identified, the
              ref_point_set flag was set to false and this method already
              exited.*/
    jeod::Vector3::copy(reference_data.direction,
                  origin_direction);
    jeod::Vector3::cross( origin_direction,
                    origin_position_unit_pfix,
                    dir_x_pos_unit_pfix);
    jeod::Vector3::normalize( dir_x_pos_unit_pfix);
    /* If origin_direction and origin_position_unit_pfix are parallel (i.e
       origin_direction is topocentric-vertical), the cross product,
       dir_x_pos_unit_pfix, will be the zero-vector; normalization of this
       zero-vector will have no effect, and dir_x_pos_unit_pfix will remain
       a zero-vector.
       When it is used in RangeComputation::update_using_cartesian(), the
       arc-sine of zero will assign zero to the crossrange angle, resulting
       in the downrange angle evaluating to be equal to the total range
       angle.*/
    if (MathUtils::is_near_equal(jeod::Vector3::vmag(dir_x_pos_unit_pfix), 0.0)) {
      CMLMessage::warn(__FILE__, __LINE__, "Invalid reference direction\n",
        "The reference direction is the topocentric zenith or nadir at the "
        "reference position.\n"
        "\"Downrange\" and \"crossrange\" are undefined.\n"
        "Crossrange angle will always be 0 and downrange angle will always\n"
        "equal the total angle.\n");
    }
  }
}

/*****************************************************************************
update_using_polar
Purpose:(Updates the range using the polar values - longitude, latitude, and a
         direction specified by azimuth)
*****************************************************************************/
void
RangeFromPfixReference::update_using_polar()
{
  // sin and cos of geocentric latitude.
  target_sin_lat = std::sin(state.sphere_coords.latitude);
  target_cos_lat = std::cos(state.sphere_coords.latitude);
  target_longitude = state.sphere_coords.longitude;

  RangeComputation::update_using_polar();
}

/*****************************************************************************
update_using_cartesian
Purpose:(Updates the range using the cartesian values)
*****************************************************************************/
void
RangeFromPfixReference::update_using_cartesian()
{
  jeod::Vector3::scale(state.cart_coords,
                 (1/current_radius),
                 target_position_unit_pfix);

  RangeComputation::update_using_cartesian();
}
