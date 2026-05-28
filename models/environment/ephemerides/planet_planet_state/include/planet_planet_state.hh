/******************************** TRICK HEADER **********************************
PURPOSE:
   (Generates state between two reference frames -- typically a planet-to-planet
    position state, using the planetary ephemerides.
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

LIBRARY DEPENDENCIES:
   ((../src/planet_planet_state.cc))

PROGRAMMERS:
   (((Gary Turner) (OSR) (July 2015) (Antares) (initial version))
    ((Bingquan Wang) (OSR) (May 2017) (Antares)
                           (cleaned up the code per its IV&V code review))
   )

********************************************************************************/

#ifndef ANTARES_PLANET_PLANET_STATE_HH
#define ANTARES_PLANET_PLANET_STATE_HH

#include "jeod/models/dynamics/dyn_manager/include/dyn_manager.hh"
#include "jeod/models/utils/ref_frames/include/ref_frame_state.hh"
#include "jeod/models/utils/ref_frames/include/ref_frame.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include <string>


class PlanetPlanetState : public SubscriptionBase
{
 public:
  // computes the state from "from-frame" to "to-frame", expressed in
  // "from-frame".
  std::string from_frame_name; /* (--) the name of the "from" frame */
  std::string to_frame_name;   /* (--) the name of the "to" frame */
  bool position_vector_only; /* (--) only need position. */

  jeod::RefFrameState state; /* (--) OUTPUT state. */

 protected:
  jeod::DynManager & dyn_manager; /* (--) Dynamics manager reference. */

  jeod::RefFrame * from_frame; /* (--) state from this frame */
  jeod::RefFrame * to_frame;   /* (--) state to this frame */
  bool init_from_name; /* (--) Ref frames set by name */

 public:
  explicit PlanetPlanetState( jeod::DynManager & dyn_mgr);
  PlanetPlanetState( jeod::DynManager & dyn_mgr,
                     jeod::RefFrame & to_frame,
                     jeod::RefFrame & from_frame);
  virtual ~PlanetPlanetState(){};

  void initialize();
  void update();

 protected:
  void activate();
  void deactivate();

 private:
   // Not implemented:
   PlanetPlanetState (const PlanetPlanetState&);
   PlanetPlanetState& operator = (const PlanetPlanetState&);
};
#endif
