/******************************** TRICK HEADER **********************************
PURPOSE:
   (Interface for the Simple Winds basic table-driven model.)

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (July 2018) (ANTARES)
       (New implementation))
   )
********************************************************************************/
#define _USE_MATH_DEFINES // M_PI
#include <cmath>           // M_PI, fmod
#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/atmosphere_exec_simple_winds.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
AtmosphereExec_SimpleLookupWind::AtmosphereExec_SimpleLookupWind(
           SimpleLookupWind              & wind_table_in,
           ExtendedPlanetaryDerivedState & planet_state_in,
           AtmosExecOutput               & master_output)
  :
  AtmosphereExec_AtmosWindsBase(master_output, planet_state_in),
  wind_table(wind_table_in),
  wind_components_type(Topodetic),
  altitude_type(Topodetic),
  altitude_at_prev_update(0.0),
  altitude_ptr(&planet_state.state.ellip_coords.altitude)
{
  name = "SimpleLookup";
}

/*****************************************************************************
activate
Purpose:(Makes sure all dependencies are in place before executing)
*****************************************************************************/
void
AtmosphereExec_SimpleLookupWind::activate()
{
  if (active) return; // no action needed

  if (altitude_type == Topocentric) {
    subscribe_to_topocentric_altitude();
  }
  wind_table.subscribe();
  AtmosphereExec_AtmosBase::activate();
}

/*****************************************************************************
deactivate
Purpose:(Removes subscriptions)
*****************************************************************************/
void
AtmosphereExec_SimpleLookupWind::deactivate()
{
  if (!active) return; // no action needed.

  if (altitude_type == Topocentric ) {
    planet_state.unsubscribe_topocentric_altitude();
  }
  wind_table.unsubscribe();
  AtmosphereExec_AtmosBase::deactivate();
}

/*****************************************************************************
update_winds
Purpose:(Executes the table lookup, then takes the output from the
         table-lookup and applies it to the AtmosExecOut target.)
*****************************************************************************/
void
AtmosphereExec_SimpleLookupWind::update_winds()
{
  if (!active) return;

  // Quick check that update is necessary:
  // NOTE - altitude_ptr is set at construction; it may be reset via
  // set_altitude_type, but it cannot be set to NULL.
  if (!MathUtils::is_near_equal(altitude_at_prev_update, *altitude_ptr)) {
    altitude_at_prev_update = *altitude_ptr;
    wind_table.update(altitude_at_prev_update);
  }

  out.wind_angle_blowing_from = wind_table.wind_blowing_from;
  out.wind_angle_blowing_to   = std::fmod( wind_table.wind_blowing_from + M_PI,
                                           2*M_PI);
  out.wind_vmag               = wind_table.wind_magnitude;

  if (wind_components_type == Topodetic) {
    jeod::Vector3::copy( wind_table.wind_component,
                   out.wind_velocity_td);
    winds_from_topodetic();
  }
  else { // Topocentric or Spherical; these frames are aligned.
    jeod::Vector3::copy( wind_table.wind_component,
                   out.wind_velocity_tc);
    winds_from_topocentric();
  }
}

/*****************************************************************************
set_altitude_type
Purpose:(Sets how the table altitude values are to be interpreted, and
         assigns the altitude pointer to the correct variable.)
*****************************************************************************/
void
AtmosphereExec_SimpleLookupWind::set_altitude_type(
    TopoType type)
{
  bool tc_before = altitude_type == Topocentric;

  switch( type) {
  case Topodetic:
    altitude_ptr = &planet_state.state.ellip_coords.altitude;
    break;
  case Topocentric:
    altitude_ptr = &planet_state.topocentric_altitude;
    break;
  case Spherical:
    altitude_ptr = &planet_state.state.sphere_coords.altitude;
    break;
  default:
    CMLMessage::error(__FILE__, __LINE__, "Invalid altitude type\n",
      "set_altitude_type() called with invalid type. altitude_type will\n"
      "remain at its previous value.");
    return;
  }
  altitude_type = type;

  bool tc_after = altitude_type == Topocentric;
  if (active) {
    if (tc_before && !tc_after) {
      planet_state.unsubscribe_topocentric_altitude();
    }
    else if (!tc_before && tc_after) {
      planet_state.subscribe_topocentric_altitude();
      planet_state.update();
    }
  }
}