/******************************** TRICK HEADER **********************************
PURPOSE:
   (Input/output interface for the atmosphere executive.)

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (May 2016) (ANTARES)
       (New architecture to minimize the complexity of the interface that has
       to be seen by other models.))
   )
********************************************************************************/

#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/atmosphere_exec_interface.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
AtmosphereExecInterface::AtmosphereExecInterface(
    const double & dyn_time_in,
    ExtendedPlanetaryDerivedState & planet_state_in,
    const jeod::DynBody  & body_in)
  :
  dyn_time( dyn_time_in),
  planet_state( planet_state_in),
  body( body_in),
  current_atmos(&atmos_none),
  current_winds(&atmos_none),
  atmos_none(out, planet_state),
  subscribed_planet_state_topo_calcs(false),
  out()
{
  subscribe_name = "AtmosphereExecInterface:";
}

/*****************************************************************************
activate
Purpose:(Activates the model and its dependencies)
*****************************************************************************/
void
AtmosphereExecInterface::activate()
{
  // initialize the wind-output in the case that no wind is specified.
  if (current_winds == &atmos_none) {
    out.wind_angle_blowing_from = 0.0;
    out.wind_angle_blowing_to   = 0.0;
    out.wind_vmag               = 0.0;
    jeod::Vector3::initialize( out.wind_velocity_tc);
    jeod::Vector3::initialize( out.wind_velocity_td);
    jeod::Vector3::initialize( out.wind_velocity_eci);
  }
  // Otherwise, add topocentric and topodetic computations to planet-rel-state
  // All wind models require topocentric and topodetic transformation
  // matrices.  If the winds are successfully initialized, ensure that these
  // values get computed.
  else {
    planet_state.subscribe_topocentric();
    planet_state.subscribe_topodetic();
    subscribed_planet_state_topo_calcs = true;
  }

  // Now subscribe to the planet-rel-state.
  // Note - if the planet-rel-state is already active, this will just add one
  //        to its subscription count.  Otherwise, it will activate it AND
  //        execute it.  Now, we need it to execute, so if it is already active
  //        force it to execute.
  bool planet_state_already_active = planet_state.is_active();
  planet_state.subscribe();
  if (planet_state_already_active) {
    planet_state.update();
  }
  SubscriptionBase::activate();
}

/*****************************************************************************
deactivate
Purpose:(Deactivates the model and its dependencies)
*****************************************************************************/
void
AtmosphereExecInterface::deactivate()
{
  if (active) {
    if ( subscribed_planet_state_topo_calcs) {;
      subscribed_planet_state_topo_calcs = false;
      planet_state.unsubscribe_topocentric();
      planet_state.unsubscribe_topodetic();
      out.zero_outputs();
    }
    planet_state.unsubscribe();
    SubscriptionBase::deactivate();
  }
}

/*****************************************************************************
initialize
Purpose:()
*****************************************************************************/
void
AtmosphereExecInterface::initialize()
{
  if (current_atmos == NULL ||
      current_winds == NULL) {
    CMLMessage::fail(
      __FILE__,__LINE__,"Error in initialization sequence.\n",
      "Pointers to current wind or atmosphere model were set to NULL\n"
      "after construction time.\n"
      "This should never happen.  Something went badly wrong.\n");
    return;
  }

  // If an atmosphere was specified and it fails to initialize, reset the
  // specification
  if (  current_atmos != &atmos_none &&
       !current_atmos->initialize_atmos() ){
    CMLMessage::error(
      __FILE__,__LINE__,"Atmosphere initialization failed.\n",
      "The initialization of the specified atmosphere failed.\n"
      "Atmosphere removed.\n");
    current_atmos = &atmos_none;
  }

  // If a winds model was specified and it fails to initialize, reset the
  // specification
  if (  current_winds != &atmos_none &&
       !current_winds->initialize_winds()) {
    CMLMessage::error(
    __FILE__,__LINE__,"Atmosphere initialization failed.\n",
    "The initialization of the specified atmosphere failed.\n"
    "Atmosphere removed.\n");
    current_winds = &atmos_none;
  }
  SubscriptionBase::initialize();
}
