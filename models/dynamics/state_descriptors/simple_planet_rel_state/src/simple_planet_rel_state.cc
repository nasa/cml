/*******************************TRICK HEADER******************************
PURPOSE: (A really simple extension of relative derived state. 
          Separation state is for two dyn-bodies, something else was needed
          for planet->body state.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Apr 2017) (Antares) (first cut)))
**********************************************************************/

#include <string>

#include "../include/simple_planet_rel_state.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
SimplePlanetRelState::SimplePlanetRelState(
     const std::string body_frame_name_,
     const std::string planet_frame_name_)
  :
  SubscriptionBase(),
  RelativeDerivedState(),
  body_frame_name(body_frame_name_),
  planet_frame_name(planet_frame_name_),
  reference_radius(0.0),
  altitude(0.0)

{
  subscribe_name = "SimplePlanetRelState:";

  direction_sense = jeod::RelativeDerivedState::ComputeSubjectStateinTarget;
}

/*****************************************************************************
initialize
Purpose:(initializes the model)
*****************************************************************************/
void
SimplePlanetRelState::initialize(
   jeod::DynBody & subject_body,
   jeod::DynManager & dyn_manager)
{
  if (!enabled) {
    return;
  }
  // Set subject-frame-name and target-frame-name temporarily for the
  // RelDerState initialization.
  subject_frame_name = body_frame_name;
  target_frame_name = planet_frame_name;
  jeod::RelativeDerivedState::initialize( subject_body, dyn_manager);

  SubscriptionBase::initialize();
}


/*****************************************************************************
update
Purpose:(Computes the state)
*****************************************************************************/
void
SimplePlanetRelState::update()
{
  if (!SubscriptionBase::active) {
    return;
  }

  jeod::RelativeDerivedState::update();
  altitude = jeod::Vector3::vmag(rel_state.trans.position) - reference_radius;
}
