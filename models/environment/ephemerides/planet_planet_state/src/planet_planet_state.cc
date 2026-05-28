/******************************** TRICK HEADER **********************************
PURPOSE:
   (Generates state between two reference frames -- typically a planet-to-planet
    position state, using the planetary ephemerides.i
    This functionality would typically be accessed directly by a
    model, as and when it is required.
    However, to support operation in sim architectures that do not support this
    on-demand lookup, some models require that the values be generated
    externally and be made available as simple arrays.
    This model uses the jeod::RefFrameState structure to hold the state between two
    frames in support of such models.
    Note - cannot simply use RelativeDerivedState because that does require
           a BodyRefFrame; this class is for relative state between two
           non-body-frames)

PROGRAMMERS:
   (((Gary Turner) (OSR) (July 2015) (Antares) (initial version)))

********************************************************************************/

#include <cstring> // NULL
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/planet_planet_state.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
PlanetPlanetState::PlanetPlanetState(
   jeod::DynManager & dyn_mgr_in)
   :
   from_frame_name(),
   to_frame_name(),
   position_vector_only(true),
   state(),
   dyn_manager(dyn_mgr_in),
   from_frame(NULL),
   to_frame(NULL),
   init_from_name(true)
{
  subscribe_name = "PlanetPlanetState:";
}
//*****************************************************************************
PlanetPlanetState::PlanetPlanetState(
   jeod::DynManager & dyn_mgr_in,
   jeod::RefFrame & to_frame_in,
   jeod::RefFrame & from_frame_in)
   :
   from_frame_name(),
   to_frame_name(),
   position_vector_only(true),
   state(),
   dyn_manager(dyn_mgr_in),
   from_frame(&from_frame_in),
   to_frame(&to_frame_in),
   init_from_name(false)
{
  subscribe_name = "PlanetPlanetState:";
}

/*****************************************************************************
initialize
Purpose:(Sets pointers)
*****************************************************************************/
void
PlanetPlanetState::initialize()
{
   if (init_from_name) {
     if (from_frame_name.empty()) {
      CMLMessage::fail (
         __FILE__, __LINE__, "PlanetPlanetState::invalid name\n",
         "Missing from_frame_name for PlanetPlanetState.\n"
         "(corresponding to_frame_name for identification is '",
         to_frame_name, "')");
      return;
     }
     // Note -- can use from_frame_name in next message because it is non-null
     // by previous check.
     if (to_frame_name.empty()) {
      CMLMessage::fail (
         __FILE__, __LINE__, "PlanetPlanetState::invalid name\n",
         "Missing to_frame_name for PlanetPlanetState.\n"
         "(corresponding from_frame_name for identification is '",
         from_frame_name, "')");
      return;
     }

    // Get the ephemeris reference frame.
    to_frame = dyn_manager.find_ref_frame( to_frame_name );
    // Get the ephemeris subject frame.
    from_frame = dyn_manager.find_ref_frame( from_frame_name );
  }

  // Ensure frames found and pointers allocated.
  // Note -- names are valid constructs, but don't name valid frames.
  if (to_frame  == NULL) {
     CMLMessage::fail (
        __FILE__, __LINE__, "PlanetPlanetState::invalid name\n",
        "Invalid to-frame '", to_frame_name, "' for PlanetPlanetState");
     return;
  }
  if (from_frame == NULL) {
     CMLMessage::fail (
        __FILE__, __LINE__, "PlanetPlanetState::invalid name\n",
        "Invalid from-frame '", from_frame_name, "' for PlanetPlanetState");
     return;
  }
  SubscriptionBase::initialize();
}

/*****************************************************************************
update
Purpose:(Computes the relative state)
*****************************************************************************/
void
PlanetPlanetState::update()
{
  if (!active) {
    return;
  }

  dyn_manager.update_ephemerides(); // timestamp-protected, runs only if not
                                    // already current

  if (position_vector_only) {
    to_frame->compute_position_from( *from_frame, state.trans.position);
  }
  else {
    to_frame->compute_relative_state( *from_frame, state );
  }
}

/*****************************************************************************
activate
Purpose:(Subscribes frames)
*****************************************************************************/
void
PlanetPlanetState::activate()
{
  // Can only be called from subscribe()
  // Requires initialized = true => pointers are non-NULL.

  // Ensure the target frame is computed.
  from_frame->subscribe();
  to_frame->subscribe();
  SubscriptionBase::activate();
  update();
}

/*****************************************************************************
deactivate
Purpose:(turns the model off)
*****************************************************************************/
void
PlanetPlanetState::deactivate()
{
  // Can only be called from unsubscribe() only if already active
  // active requires initialized = true => pointers are non-NULL.
  from_frame->unsubscribe();
  to_frame->unsubscribe();
  SubscriptionBase::deactivate();
}
