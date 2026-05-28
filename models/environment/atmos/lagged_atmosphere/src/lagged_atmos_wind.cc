/*******************************************************************************
Purpose:
   (Define methods for the class LaggedAtmosWind.)

Author:
 (((Gary Turner) (OSR) (Jan 2021) (Antares)
    (Based on LaggedAtmosphere and LaggedWinds by Jeff Semrau,
     written to support EG-chutes code.))
  )
*******************************************************************************/

#include "../include/lagged_atmos_wind.hh"

#include "jeod/models/utils/math/include/vector3.hh"

#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include <string>
/*****************************************************************************
Constructors
*****************************************************************************/
LaggedAtmosWind::LaggedAtmosWind()
  :
  data_out(),
  model_name("LaggedAtmosWind"),
  min_delta_altitude(1E-2), /* essentially 0, while guaranteeing uniqueness
                               of the node altitudes. */
  max_delta_altitude(75),
  initialized(false),
  nodes(),
  above()
{
  // mark that data_out is invalid, this will be assigned when data_out
  // is populated.
  data_out.altitude = -1E10;
}
/****************************************************************************/
LaggedAtmosWind::LaggedAtmosWind( const std::string & name_)
  :
  LaggedAtmosWind()
{
  model_name = name_;
}

/*****************************************************************************
compute_with_query
Purpose:
  Executes compute only if the input altitude is different from the last time
  this method was run.
*****************************************************************************/
void
LaggedAtmosWind::compute_with_query( double input_altitude)
{
  if (!MathUtils::is_near_equal( input_altitude,
                                 data_out.altitude)) {
    compute( input_altitude);
  }
  // otherwise, return silently; data is current.
}

/*****************************************************************************
compute
Purpose:
  Based on the input altitude, interpolate the history data to generate
  approximate values for the atmospheric conditiions at the currrent altitude.
*****************************************************************************/
void
LaggedAtmosWind::compute( double input_altitude)
{
  if (!initialized) {
    CMLMessage::fail( __FILE__, __LINE__,
    "Atmosphere queried before initialization in ",model_name,".\n");
  }

  // Unreachable code in current implementation; nodes is protected, it has
  // one entry if initialized, and does not pop off its last set.
  if (nodes.empty()) {
    CMLMessage::fail(__FILE__,__LINE__,
    "There are no nodes in the list in model ",model_name,".\n");
  }

  if (nodes.size() == 1) {
    CMLMessage::warn(__FILE__, __LINE__,
    "Unable to bound altitude in model ",model_name,".\n"
    "There is only 1 node on which to base the output.\n\n");
    // pull the data from the only node, except for altitude which comes from
    // the input.
    data_out = nodes.front();
    data_out.altitude = input_altitude;
    return;
  }

  // While we expect the altitude of the vehicle to decrease, the flyout
  // behavior of the chutes does allow them to increase in altitude in some
  // circumstances.  So first we'll search backwards to make sure the above
  // iterator is indeed AT OR ABOVE the current altitude, and then we'll
  // search forwards to make sure the below iterator is indeed AT OR BELOW
  // the current altitude.

  while ( input_altitude > above->altitude &&
          above != nodes.begin() ) {
    // Decrement "above" moves it to the left in the list, to a higher altitude
    --above;
  }

  // create an iterator to the next point in the list.
  // Start at the same node as the "above" iterator to detect the case
  // where the input altitude is above the node table.
  auto below = above;

  // Now push this iterator through the list until it points to an altitude
  // below input_altitude. Note that this could result in "below" pointing
  // to nodes.end(), which isn't an actual node.
  while ( input_altitude < below->altitude &&
          below != nodes.end() ) {
    // Increment "below" moves it to the right in the list, to a lower altitude
    ++below;
  }

  // Now check whether the input altitude is outside the domain provided by
  // the node table.
  // If "below" is still the first node, that means the first node is below
  // the input altitude, so the input altitude is above the node table.
  // In this case, set below to the next node and extrapolate instead of
  // interpolate.
  if (below == nodes.begin()) {
    ++below;
    CMLMessage::warn(__FILE__, __LINE__,
    "Unable to bound altitude in model ",model_name,".\n"
    "Input altitude ",input_altitude," is at or above the top of the node "
    "table.\n"
    "Extrapolating based on node-altitudes ",above->altitude," and ",
    below->altitude,".\n");
  }
  // If "below" is pointing to something beyond the last node, i.e. end(),
  // that means the last node node is above the input altitude,
  // so the input altitude is below the node table.
  // In this case, set "below" to the last node, "above" to the next-oto-last
  // node, and extrapolate instead of interpolate.
  else if (below == nodes.end()) {
    --below;
    above = std::prev(below,1);
    CMLMessage::warn(__FILE__, __LINE__,
    "Unable to bound altitude in model ",model_name,".\n"
    "Input altitude ",input_altitude," is below the node table.\n"
    "Extrapolating based on node-altitudes ",above->altitude," and ",
    below->altitude,".\n");
  }
  // Otherwise the input altitude is in the table, move "above" to be one spot
  // to the left of "below"
  else {
    // Then set the "above" iterator to be one location above the "below"
    // iterator, thereby bounding the altitude between "above" and "below"
    above = std::prev(below,1);
  }

  // Now we linearly interpolate to get the atmosphere at the current
  // altitude.
  // NOTE -- To be fully protected, we have to use
  //         MathUtils::divide_protected to guard against overflow in case
  //         below and above are really close together
  //         The fail-safe setting, frac = 0, results in using the
  //         parameters from the "above" iterator.
  double frac = MathUtils::divide_protected(
                                  (input_altitude - above->altitude),
                                  (below->altitude - above->altitude),
                                  0,
                                  false);
  data_out.altitude = input_altitude;
  data_out.density = above->density + frac * (below->density - above->density);
  data_out.speed_of_sound = above->speed_of_sound +
                   frac * (below->speed_of_sound - above->speed_of_sound);
  for (size_t i = 0; i < 3; ++i) {
    data_out.planetodetic_wind_velocity[i] =
                               above->planetodetic_wind_velocity[i] +
                               frac * (below->planetodetic_wind_velocity[i] -
                                       above->planetodetic_wind_velocity[i]);
  }
}

/*****************************************************************************
Creates another node with the passed-in payload data and removes nodes that
are out-of-reach.
*****************************************************************************/
void
LaggedAtmosWind::update_history( const LaggedAtmosPayloadData &payload_data)
{
  // Add the current payload state to the node list under the following
  // conditions:
  //  - if the nodes list has not yet been populated (in which case it has no
  //    entries to evaluate against the current state)
  //  OR
  //  - Both of these conditions:
  //    - if the payload altitude has moved sufficiently lower than the lowest
  //      calibrated point in nodes (nodes must be physically seperated to
  //      support interpolation between them)
  //    AND
  //    - the atmospheric state has been updated since the previous node (the
  //      atmosphere likely updates at a slower rate than the kinematic state
  //      (i.e. altitude) and we do not want a stair-step profile of atmospheric
  //      state, with periods of constant values and periodic steps).
  if (initialized) {
    if ((payload_data.altitude <= nodes.back().altitude - min_delta_altitude) &&
        (!MathUtils::is_near_equal(payload_data.density, nodes.back().density))
       ) {
      nodes.push_back(payload_data);
    }
  } else { // first-pass
    nodes.push_back(payload_data);
    // Start the above iterator at the beginning of the node list.
    above = nodes.begin();
    initialized = true;
  }



  // We don't want to keep the nodes forever.  So delete any nodes with
  // altitudes that are farther above the current altitude than the chute can
  // possibly be.  But don't invalidate the above iterator.
  double max_alt = payload_data.altitude + max_delta_altitude;
  while (above != nodes.begin() &&
         nodes.front().altitude > max_alt) {
    nodes.pop_front();
  }
}

/*****************************************************************************
setters
*****************************************************************************/
void
LaggedAtmosWind::set_min_delta_altitude( double min_new)
{
  if (min_new <= 0.0) {
    CMLMessage::warn(__FILE__, __LINE__,
    "Illegal value for delta-altitude in model ",model_name,".\n"
    "Minimum delta-altitude must be greater than 0 to preserve\n"
    "uniqueness of the node points.\n");
  } else {
    min_delta_altitude = min_new;
  }
}
/****************************************************************************/
void
LaggedAtmosWind::set_max_delta_altitude( double max_new)
{
  if (max_new <= 0.0) {
    CMLMessage::warn(__FILE__, __LINE__,
    "Illegal value for delta-altitude in model ",model_name,".\n"
    "Maximum delta-altitude must be greater than 0 to preserve\n"
    "any node points; node points more than this value above the\n"
    "current altitude are removed.\n");
  } else {
    max_delta_altitude = max_new;
  }
}
