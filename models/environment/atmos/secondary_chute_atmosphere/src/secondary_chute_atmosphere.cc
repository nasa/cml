/*********************************************************************************
PURPOSE:
  (Provide the class to facilitate adding an extra atmosphere to a vehicle for
  the purposes of modeling the chutes.)

LIBRARY DEPENDENCY:
    ((secondary_chute_atmosphere.o))

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (2/2015) (ANTARES) (Initial version))
   )
*********************************************************************************/

#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/secondary_chute_atmosphere.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
ChuteAtmosphere::ChuteAtmosphere(
         LookupAtmosWinds   & DRWP_atmos_in,
         STD1976            & std_76_in,
         SimpleLookupWind   & simple_wind,
         GramInterface      & gram_in,
         const GramInterface & master_gram_i,
         double             & dyn_time_in,
         ExtendedPlanetaryDerivedState & veh_state_in,
         jeod::DynBody  & body_in,
         State_Atmos_Output & chute_state_in)
  :
  planet_rel_state(),
  atmos_exec( gram_in,
              DRWP_atmos_in,
              std_76_in,
              simple_wind,
              &master_gram_i,
              dyn_time_in,
              planet_rel_state,
              body_in),
  veh_state(veh_state_in),
  chute_state(chute_state_in)
{
  subscribe_name = "ChuteAtmosphere:";
}


/*****************************************************************************
initialize
Purpose:(Initializes the class)
*****************************************************************************/
void
ChuteAtmosphere::initialize(
         jeod::TimeStandard & time)
{
  if (!enabled) {
    return;
  }

  // Build the GRAM-10 atmosphere.
  // TODO When the atmosphere is handled at a planetary level, this will already be
  // done.  In which case, a pointer to the planetary-version of GRAM-10 will need to
  // be passed in to this method.

  // Initialize the atmos_exec:  Mostly a pass-through, with the local gram-10 and
  // planet-rel-state.
  atmos_exec.initialize( time);

  // Pass the command up to the base class.
  SubscriptionBase::initialize();
}

/*****************************************************************************
update
Purpose:(calls the atmos-exec for the chute-state.)
*****************************************************************************/
void
ChuteAtmosphere::update()
{
  if (!active) {
    return;
  }

  // Populate the planet-rel-state structure with the state from the
  // already-computed vehicle-based planet-rel-state and the
  // already-computed chute-state-override.
  // NOTE - to save time, just copy these data; do not recompute them.
  populate_planet_rel_state();

  // With the planet-rel-state values now set, call the atmos_exec.
  // atmos_exec was constructed with a reference to planet-rel-state.
  atmos_exec.update ();
  atmos_exec.wind_hold.update();
}

/*****************************************************************************
activate
Purpose:(The sequence for getting the secondary chute atmosphere up-and-running)
*****************************************************************************/
void
ChuteAtmosphere::activate()
{
  // Confusing code sequence coming up.  Here's why:
  // atmos_exec update relies on the planet-relative-state to execute.  In this
  // case, the planet-relative-state class instance that is used is
  // planet_rel_state; it is populated from two sources - the parachute
  // model's chute-state structure and the vehicle's planet-rel-state instance,
  // called veh_state.
  // Both of these are needed to populate planet_rel_state before
  // atmos_exec.update() gets called.
  // The standard subscribe() method will only generate the veh_state instance,
  // and this class's update method will push those data correctly.  however, we
  // still need the chute_state data populating.
  // Solution is to subscribe to veh_state (and update it), copy the data out of
  // there into chute_state, and then unsubscribe it.  It will be immediately
  // re-subscribed once atmos_exec getes subscribed, but that path leads to an
  // automatic atmos_exec.update, so it is too late to populate chute_state.
  veh_state.subscribe();
  chute_state.geocen_alt = veh_state.state.sphere_coords.altitude;
  chute_state.geocen_lat = veh_state.state.sphere_coords.latitude;
  chute_state.geocen_lon = veh_state.state.sphere_coords.longitude;
  chute_state.geocen_alt_gram = veh_state.topocentric_altitude;

  // Now use these data and the veh_state data to populate the planet_rel_state
  // data.
  populate_planet_rel_state();

  // With the planet-rel-state values now set, call the atmos_exec.
  // atmos_exec now subscribes to planet_rel_state, but because planet_rel_state
  // was never initialized, it will not update itself.  Disabling it ahead of the
  // subscription call would provide additional security but would also lead to
  // a warning.  Instead, disable it immediately afterwards
  // The necessary data comes from veh_state, and we just subscribed to that!
  atmos_exec.subscribe();
  planet_rel_state.disable();

  active = true;
  update();
}

/*****************************************************************************
populate_planet_rel_state
Purpose:(Populates the defunct planet_rel_state with appropriate data from the
         veh_state and the chute_state.  Atmos_exec looks to these data fori
         computation of the atmospheric state.)
*****************************************************************************/
void
ChuteAtmosphere::populate_planet_rel_state()
{
  planet_rel_state.state.ellip_coords.altitude =
                            veh_state.state.ellip_coords.altitude;
  planet_rel_state.state.ellip_coords.latitude =
                            veh_state.state.ellip_coords.latitude;
  planet_rel_state.state.ellip_coords.longitude =
                            veh_state.state.ellip_coords.longitude;
  if(veh_state.is_topodetic_subscribed())
  {
      jeod::Matrix3x3::copy( veh_state.topodetic.T_inrtl_to_this,
                       planet_rel_state.topodetic.T_inrtl_to_this);
      jeod::Matrix3x3::copy( veh_state.topodetic.T_this_to_body,
                       planet_rel_state.topodetic.T_this_to_body);
  }
  if(veh_state.is_topocentric_subscribed())
  {
      jeod::Matrix3x3::copy( veh_state.topocentric.T_this_to_body,
                       planet_rel_state.topocentric.T_this_to_body);
      jeod::Matrix3x3::copy( veh_state.topocentric.T_inrtl_to_this,
                       planet_rel_state.topocentric.T_inrtl_to_this);
  }

  // Populate the planet-rel-state topocentric structure with the state from
  // the already-computed chute-state-override.
  // NOTE - to save time, just copy these data; do not recompute them.
  planet_rel_state.state.sphere_coords.altitude = chute_state.geocen_alt;
  planet_rel_state.state.sphere_coords.latitude = chute_state.geocen_lat;
  planet_rel_state.state.sphere_coords.longitude = chute_state.geocen_lon;
  planet_rel_state.topocentric_altitude =  chute_state.geocen_alt_gram;
}