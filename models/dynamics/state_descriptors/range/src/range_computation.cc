/********************************* TRICK HEADER *******************************
Purpose:
   (Provides the common calculation set for the cases where range is computed
   from a fixed reference location and where range is computed to a fixed
   reference location.)

Reference:
  (See docs directory for equation derivations and identifying numbers)

Programmers:
   (((Gary Turner) (OSR) (Sept 2014) (Initial version))
    ((Bingquan Wang) (OSR) (April 2017) (Fixed the compilation warning of
                           float-point number equality comparison)))
*******************************************************************************/

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "jeod/models/utils/ref_frames/include/ref_frame_state.hh" // RefFrameTrans
#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/range_computation.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
RangeValues::RangeValues()
  :
  totalrange_angle(0.0),
  crossrange_angle(0.0),
  downrange_angle(0.0),
  cross_range_avg_rad(0.0),
  down_range_avg_rad(0.0),
  total_range_avg_rad(0.0),
  cross_range_ref_rad(0.0),
  down_range_ref_rad(0.0),
  total_range_ref_rad(0.0)
{}


/*****************************************************************************
Constructor
*****************************************************************************/
RangeComputation::RangeComputation( const jeod::PlanetFixedPosition & state_in )
  :
  reference_data(),
  reference_radius(0.0),
  current_radius(0.0),
  average_radius(0.0),
  ref_point_set(false),
  use_polar_direction(false),
  set_direction(false),
  ref_sin_lat(0.0),
  ref_cos_lat(0.0),
  origin_sin_lat(0.0),
  origin_cos_lat(0.0),
  target_sin_lat(0.0),
  target_cos_lat(0.0),
  origin_sin_azimuth(0.0),
  origin_cos_azimuth(0.0),
  origin_longitude(0.0),
  target_longitude(0.0),
  reference_position_unit_pfix{0.0, 0.0, 0.0},
  origin_position_unit_pfix{0.0, 0.0, 0.0},
  target_position_unit_pfix{0.0, 0.0, 0.0},
  origin_direction{0.0, 0.0, 0.0},
  cos_totalrange_angle(0.0),
  pos_x_pos_unit_pfix{0.0, 0.0, 0.0},
  dir_x_pos_unit_pfix{0.0, 0.0, 0.0},
  state(state_in)
{
  subscribe_name = "RangeComputation:";
}


/*****************************************************************************
initialize
Purpose:(simple pass-through)
*****************************************************************************/
void
RangeComputation::initialize(
       const jeod::Planet & planet_in)
{
  // Pass through to PlanetFixedPosition
  // Note - need the const-cast here because PlanetFixedPosition::initialize
  //        does not have a const in the argument even though the pointer is
  //        treated as const.
  reference_data.initialize( const_cast<jeod::Planet *>(&planet_in));
  SubscriptionBase::initialize(); // Pass up to base class.
  set_reference_data_query();
}

/*****************************************************************************
set_reference_data_query
Purpose:(Avoids error messages if user is unsure whether or not the model is
         ready to proceed with setting the reference data.)
******************************************************************************/
void
RangeComputation::set_reference_data_query()
{
  if ((reference_data.position_type != PfixReferencePoint::undefined) &&
      ((reference_data.direction_type != PfixReferencePoint::undefined) ||
       (!set_direction))) {
    set_reference_data();
  }
}

/*****************************************************************************
set_reference_data_from_inrtl_state
Purpose:(Use the position and velocity as the position and direction.)
*****************************************************************************/
void
RangeComputation::set_reference_data_from_inrtl_state(
   const jeod::RefFrameTrans & trans)
{
   set_reference_data_from_inrtl_state(trans.position, trans.velocity);
}

/*****************************************************************************
set_reference_data_from_inrtl_state
Purpose:(Pass in an inertial position and direction vector (e.g. velocity)
to define the reference position and direction)
*****************************************************************************/
void
RangeComputation::set_reference_data_from_inrtl_state(
   const double position_in[3],
   const double direction_in[3])
{
  reference_data.position_type = PfixReferencePoint::inertial;
  reference_data.direction_type = PfixReferencePoint::inertial;
  set_reference_data_from_state(position_in, direction_in);
}

/*****************************************************************************
set_reference_data_from_state
Purpose:(Pass in a generic position and direction vector (e.g. velocity)
to define the reference position and direction)
*****************************************************************************/
void
RangeComputation::set_reference_data_from_state(
   const double position_in[3],
   const double direction_in[3])
{
  jeod::Vector3::copy(position_in, reference_data.position);
  jeod::Vector3::normalize(direction_in, reference_data.direction);

  set_reference_data();
}

/*****************************************************************************
set_reference_data
Purpose:(sets the location and, if necessary, the direction associated with the
         reference point)
*****************************************************************************/
void
RangeComputation::set_reference_data()
{
  if (!initialized) {
    CMLMessage::error(
      __FILE__,__LINE__,"Range Computation initialization sequence error:\n",
      "Call made to set_reference_data() without the model having been "
      "initialized.\nThis method should only be called externally "
      "post-initialization.");
    return;
  }

  /* set the reference position, return via arguments the radius to the
     reference position (reference_radius) and confirmation that the value is
     valid (i.e. > 0).*/
  reference_data.set_point(reference_radius,
                           ref_point_set);

  /* if this implementation of RangeComputation requires the direction to be
     set (because it was not set in the constructor), then have
     reference_data set it.  Return via arguments identification of whether
     the direction is set by Cartesian or polar values, and confirmation
     that the setting was successful.*/
  if (set_direction) {
    reference_data.set_direction( use_polar_direction,
                                  ref_point_set);
  }

  /* If either of the position or direction settings failed, the
      ref_point_set flag will be false.  In that case, exit.  There is nothing
      more that can be done until those methods have processed successfully.*/
  if (!ref_point_set) { // If nothing outstanding
    return;
  }

  /* Compute appropriate values, to be used later in the computation of range.
     The two formulations -- polar or Cartesian -- require different support
     values, so branch here and compute only the ones that are needed. */
  if (use_polar_direction) { // If direction is set by azimuth, take the sine
                             // and cosine of the latitude once and for all
                             // time.
    ref_sin_lat = std::sin(reference_data.sphere_coords.latitude);
    ref_cos_lat = std::cos(reference_data.sphere_coords.latitude);
  }
  else {
    /* If direction specified as a vector, or not specified, then need the
       unit position vector for later computation.  Compute it here once
       and for all time.
       NOTE - div-0 protection is included here because
              reference_radius = 0 => ref_point_set = false
              in that case the method exited about 15 lines back.*/
    jeod::Vector3::scale(reference_data.cart_coords,
                         (1/reference_radius),
                         reference_position_unit_pfix);
  }
}

/*****************************************************************************
update
Purpose:(Generic update methods.  note that the update_using_* calls go to the
         derived class instances, from where control returns to the generic
         forms.)
*****************************************************************************/
void
RangeComputation::update()
{
  if (!active || !ref_point_set) {
    return;
  }

  current_radius = jeod::Vector3::vmag(state.cart_coords);

  /* Need some div-0 protection here.  If for some unknown reason the
     position has been zeroed, set all range values to 0.
     NOTE - introduction of NaN or inf will proceed without escaping to 0 */
  if (current_radius < std::numeric_limits<double>::min()) {
    crossrange_angle = 0.0;
    downrange_angle = 0.0;
    totalrange_angle = 0.0;
  }
  else {
    average_radius = (reference_radius + current_radius) / 2.0;
    if (use_polar_direction) {
      // Note - typically calls derived-class instance.
      update_using_polar();
    }
    else {
      update_using_cartesian();
    }
  }

  cross_range_avg_rad = average_radius * crossrange_angle;
  down_range_avg_rad =  average_radius * downrange_angle;
  total_range_avg_rad = average_radius * totalrange_angle;

  cross_range_ref_rad = reference_radius * crossrange_angle;
  down_range_ref_rad =  reference_radius * downrange_angle;
  total_range_ref_rad = reference_radius * totalrange_angle;
}

/*****************************************************************************
update_using_polar
Purpose:(Updates the range using the polar values - longitude, latitude, and a
         direction specified by azimuth)
*****************************************************************************/
void
RangeComputation::update_using_polar()
{
  // longitude between "here" and reference point
  double delta_longitude = target_longitude - origin_longitude;
  double sin_del_long = std::sin(delta_longitude);
  double cos_del_long = std::cos(delta_longitude);

  // equation 12:
  double cos_theta_sin_tau = target_sin_lat * origin_cos_lat -
                  target_cos_lat * origin_sin_lat * cos_del_long;
  // equation 13:
  double sin_theta_sin_tau = target_cos_lat * sin_del_long;

  // total-range angle, equation 7:
  double cos_totalrange_angle = origin_sin_lat * target_sin_lat +
                                origin_cos_lat * target_cos_lat * cos_del_long;
  totalrange_angle = MathUtils::acos_protected(cos_totalrange_angle);


  // cross-range angle, equation 15:
  crossrange_angle = MathUtils::asin_protected(
                                       sin_theta_sin_tau * origin_cos_azimuth -
                                       cos_theta_sin_tau * origin_sin_azimuth);

  // downrange angle, equation 16:
  downrange_angle = std::atan2( (cos_theta_sin_tau * origin_cos_azimuth +
                                 sin_theta_sin_tau * origin_sin_azimuth),
                                cos_totalrange_angle);
}

/*****************************************************************************
update_using_cartesian
Purpose:(Updates the range using the cartesian values - a 3-vector for position
         and another for direction.)
*****************************************************************************/
void
RangeComputation::update_using_cartesian()
{
  // Take the dot product of the position unit vector and the reference
  // position unit vector.  This is the cosine of the total-range angle.
  // Compute total range, this is velocity independent.

  // equation 19:
  cos_totalrange_angle = jeod::Vector3::dot( origin_position_unit_pfix,
                                       target_position_unit_pfix);
  totalrange_angle = MathUtils::acos_protected( cos_totalrange_angle);

  // Take the cross-product of the two positions to get a vector perpendicular
  // to the plane containing the current and target position.
  jeod::Vector3::cross(target_position_unit_pfix,
                 origin_position_unit_pfix,
                 pos_x_pos_unit_pfix);
  jeod::Vector3::normalize(pos_x_pos_unit_pfix);

  // equation 20:
  crossrange_angle = MathUtils::asin_protected(
                                 jeod::Vector3::dot( target_position_unit_pfix,
                                               dir_x_pos_unit_pfix));

  double cos_crossrange_angle = std::cos(crossrange_angle);

  const double tmp_val = MathUtils::divide_protected(cos_totalrange_angle,
                                                     cos_crossrange_angle,
                                                     1.0);
  // equation 21:
  downrange_angle = MathUtils::acos_protected(tmp_val);
  if (jeod::Vector3::dot(pos_x_pos_unit_pfix,dir_x_pos_unit_pfix) < 0) {
    downrange_angle *= -1.0;
  }
}
