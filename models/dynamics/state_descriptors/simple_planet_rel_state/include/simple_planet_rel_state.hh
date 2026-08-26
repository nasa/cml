/*******************************TRICK HEADER******************************
PURPOSE: (A really simple extension of relative derived state.
          Separation state is for two dyn-bodies, something else was needed
          for planet->body state.)

LIBRARY DEPENDENCY:
   ((../src/simple_planet_rel_state.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (Apr 2017) (Antares) (first cut)))
**********************************************************************/

#ifndef CML_SIMPLE_PLANET_REL_STATE_HH
#define CML_SIMPLE_PLANET_REL_STATE_HH

#include <string>
#include "jeod/models/dynamics/derived_state/include/relative_derived_state.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

class SimplePlanetRelState : public SubscriptionBase,
                             public jeod::RelativeDerivedState
{
 protected:
  std::string body_frame_name; /* (--)
         name of the body-frame, not including the name of the body.
         E.g. "composite_body", "structure", etc.*/
  std::string planet_frame_name; /* (--)
         name of the planet reference frame, including the name of the planet
         E.g. Earth.inertial, Moon.pfix. */
 public:
  double reference_radius; /* (m) Reference-radius for computing the altitude*/
  double altitude;         /* (m) height of the frame-of-interest above the
                                  reference-height. */

  SimplePlanetRelState(const std::string & body_frame_name_,
                       const std::string & planet_frame_name);

  void initialize(jeod::DynBody & subject_body,
                          jeod::DynManager & dyn_manager) override;
  void update() override;
 private:
  SimplePlanetRelState (const SimplePlanetRelState&);
  SimplePlanetRelState & operator = (const SimplePlanetRelState&);
};
#endif
