/*******************************TRICK HEADER******************************
PURPOSE: (A generic model to simulate reverse-flow effects)

REFERENCE:
   ((Derived from Orion Subsonic wake model for Orion by Mark McPherson)
    (CM Wake Recirculation Zone CFD Report - EG-CAP-10-134 (11/3/2010))
    (Orion Sim Data Book (06/06/2011) - Reference drag area (Cd*S_ref) for FBC))

PROGRAMMERS:
  (((Mark McPherson) (NASA/DM42) (Apr 2012) (ANTARES) (Foundation))
   ((Gary Turner) (OSR) (Mar 2016) (Antares)
                (New adaptation to meet Antares coding standards))
   ((Bingquan Wang) (OSR) (Aug 2017) (Antares)
                          (cleanup per table_interp_cpp refactoring))
   ((Daniel Ghan) (OSR) (Jun 2020) (Antares) (Simplified user interface)))

NOTES:
     Body A - The wake-producing body
     Body B - The object inside the wake
**********************************************************************/

#include <cmath> //sqrt
#include "../include/wake_effects.hh"

/*****************************************************************************
constructor
*****************************************************************************/
WakeReverseFlow::WakeReverseFlow(
     const WakeGeneratingBody & objectA_in,
     const WakeFollowingBody & objectB_in,
     const WakeParamsSub & params_in)
   :
   WakeEffectsBase( objectA_in,
                    objectB_in,
                    params_in),
   Qrev(0.0),
   Qrev_lookup() // uses offset_trail_dist, sets Qrev
{}

/*****************************************************************************
initialize
Purpose:(initializes the lookup table)
*****************************************************************************/
void WakeReverseFlow::initialize()
{
  if (!enabled || !data_loaded) {
    return;
  }

  Qrev_lookup.load_independent_data(offset_trail_dist, offset_trail_dist_axis);
  Qrev_lookup.load_dependent_data(Qrev, Qrev_table);
  Qrev_lookup.initialize();
  Qrev_lookup.subscribe();
  // WakeEffectsBase::initialize doesn't exist, but it could in the future.
  // In the meantime, this calls SubscriptionBase::initialize().
  WakeEffectsBase::initialize();
}

/*****************************************************************************
update
Purpose:(Updates the model)
*****************************************************************************/
void
WakeReverseFlow::update()
{
  if (!active) {
    return;
  }

  generate_trail_and_radial_dist();

  if (in_region && objectB.get_generate_force()) {
    Qrev_lookup.update();

    // NOTES - looks like PRF version but different constant and sign
    //         difference on axial_sep_vel, so keep independent.
    //       - Qrev is only applied to modifying the freestream velocity, not
    //         the axial separation velocity, which means it cannot be applied
    //         directly to the dynamic pressure and must be applied as a
    //         square-root to the velocity term.  Thus we need protection that
    //         Qrev > 0 before taking the square-root.
    eff_wake_vel = objectB.axial_sep_vel;
    if (Qrev > 0) {
      eff_wake_vel += objectA.freestream_vel_mag * std::sqrt( Qrev);
    }
    force_toward_A = 0.5 * objectA.freestream_density *
                   eff_wake_vel * eff_wake_vel * objectB.get_drag_area();
  }
  else {
    Qrev = 0;
  }
}

/*****************************************************************************
deactivate
Purpose:(deactivates the model)
*****************************************************************************/
void
WakeReverseFlow::deactivate()
{
  Qrev = 0.0;
  WakeEffectsBase::deactivate();
}
