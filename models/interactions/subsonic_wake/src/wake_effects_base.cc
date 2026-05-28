/*******************************TRICK HEADER******************************
PURPOSE: (A generic model to simulate the common elements of the
          reverse-flow and pressure-recovery fraction (PRF) of the wake.)

REFERENCE:
   ((Derived from Orion Subsonic wake model for Orion by Mark McPherson)
    (CM Wake Recirculation Zone CFD Report - EG-CAP-10-134 (11/3/2010))
    (Orion Sim Data Book (06/06/2011) - Reference drag area (Cd*S_ref) for FBC))

PROGRAMMERS:
  (((Mark McPherson) (NASA/DM42) (Apr 2012) (ANTARES) (Foundation))
   ((Gary Turner) (OSR) (Mar 2016) (Antares)
                (New adaptation to meet Antares coding standards)))

NOTES:
     Body A - The wake-producing body
     Body B - The object inside the wake
**********************************************************************/

#include <cmath> // sin, cos
#include "../include/wake_effects.hh"
/*****************************************************************************
constructor
*****************************************************************************/
WakeEffectsBase::WakeEffectsBase(
     const WakeGeneratingBody & objectA_in,
     const WakeFollowingBody & objectB_in,
     const WakeParamsSub & params_in)
   :
   SubscriptionBase(),
   objectA(objectA_in),
   objectB(objectB_in),
   params(params_in),
   grid_offset(0.0),
   force_toward_A(0.0),
   data_loaded(false),
   in_region(false),
   offset_trail_dist(0.0),
   offset_radial_dist(0.0),
   eff_wake_vel(0.0)
{
  subscribe_name = "WakeEffectsBase:";
}

/*****************************************************************************
generate_trail_and_radial_dist
Purpose:(Calculates offset_trail_dist and offset_radial_dist)
*****************************************************************************/
void
WakeEffectsBase::generate_trail_and_radial_dist()
{
  // trailing distance - offset * cos (pi - total_alpha)
  // offset_trail_dist = objectB.trailing_distance +
  //                     grid_offset * std::cos( objectA.total_alpha);
  // NOTE:
  //  There is a fundamental diference between trailing_distance and
  //    radial_distance.  See documentation section 3.3 "Query Model
  //    Boundaries" for more discussion on this topic.
  //  trailing_distance is more like a displacement, it can be
  //    computed to have a negative sign so it is valid to add the
  //    displacement (offset * cos(alpha)) to it.
  //  radial_distance is a scalar distance.  it is not valid to add (or
  //    subtract) the displacement (offset * sin(alpha)) to a scalar distance
  //    because we do not know whether the addition will extend the existing
  //    distance or reduce it -- and even carry it back through 0 and out on
  //    the other side.
  //  Instead, we must recompute the radial distance.  We do know the vector
  //    from the original grid point to objectB, expressed in objectA's
  //    body-frame.  The equivalent offset vector requires subtracting the
  //    grid-offset on the x-axis of the body-frame.
  //        vec s' = vec s - [offset, 0,0]
  double pos_wrt_offset_grid_in_bodyA[3];
  jeod::Vector3::copy(objectB.pos_wrt_grid_in_bodyA,
                pos_wrt_offset_grid_in_bodyA);
  pos_wrt_offset_grid_in_bodyA[0] -= grid_offset;

  // Now we can obtain the offset_trail_distance and offset radial_distance
  // without resorting to trig functions:
  double offset_sep_dist = jeod::Vector3::vmag( pos_wrt_offset_grid_in_bodyA);
  offset_trail_dist = -jeod::Vector3::dot(  pos_wrt_offset_grid_in_bodyA,
                                      objectA.unitvec_freestream);
  offset_radial_dist = std::sqrt( (offset_sep_dist * offset_sep_dist) -
                                  (offset_trail_dist * offset_trail_dist));

  in_region = ((offset_trail_dist >  params.start_dist) &&
               (offset_trail_dist <= params.stop_dist) &&
               (offset_radial_dist < params.radial_dist));
  force_toward_A = 0.0;
}
