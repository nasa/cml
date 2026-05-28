/*******************************TRICK HEADER******************************
PURPOSE: (A generic model to simulate the pressure-recovery fraction (PRF)
          of the wake.)

REFERENCE:
   ((Derived from Orion Subsonic wake model for Orion by Mark McPherson)
    (CM Wake Recirculation Zone CFD Report - EG-CAP-10-134 (11/3/2010))
    (Orion Sim Data Book (06/06/2011) - Reference drag area (Cd*S_ref) for FBC))

PROGRAMMERS:
  (((Mark McPherson) (NASA/DM42) (Apr 2012) (ANTARES) (Foundation))
   ((Gary Turner) (OSR) (Mar 2016) (Antares)
                (New adaptation to meet Antares coding standards))
   ((Daniel Ghan) (OSR) (Jun 2020) (Antares) (Simplified user interface)))

NOTES:
     Body A - The wake-producing body
     Body B - The object inside the wake
**********************************************************************/

#include <cmath> //sqrt
#include <algorithm> //max
#include "../include/wake_effects.hh"

/*****************************************************************************
constructor
*****************************************************************************/
WakePrfModel::WakePrfModel(
     const WakeGeneratingBody & objectA_in,
     const WakeFollowingBody & objectB_in,
     const WakeParams & params_in)
   :
   WakeEffectsBase( objectA_in,
                    objectB_in,
                    params_in.prf),
   main_body_diameter(params_in.main_body_diameter),
   effective_area_sf(0.49), /* values around 0.5 have been found useful;
                               value of 0.49 is a convenient value to get a
                               nice round number when taking the square root.*/
   PRF_mean(1.0),
   PRF(1.0),
   unc_factor(0),
   trail_diameters(0.0),
   effective_diam(0.0),
   table_set(),
   indep_mach( "mach", objectA.freestream_mach),
   indep_alpha( "alpha", objectA.total_alpha),
   indep_trail_diam( "trail diam", trail_diameters),
   indep_eff_diam( "effective diam", effective_diam),
   prf_lookup( PRF_mean)
{}

/*****************************************************************************
initialize
Purpose:(initializes the lookup table)
*****************************************************************************/
void
WakePrfModel::initialize()
{
  if (!enabled || !data_loaded) {
    return;
  }

  // Load independent data
  indep_mach.load_data(mach_axis);
  indep_alpha.load_data(alpha_axis);
  indep_trail_diam.load_data(trail_diam_axis);
  indep_eff_diam.load_data(effective_diam_axis);

  // Load dependent data
  std::vector<size_t> dim_list;
  dim_list.push_back(1);
  dim_list.push_back(mach_axis.size());
  dim_list.push_back(alpha_axis.size());
  dim_list.push_back(trail_diam_axis.size());
  dim_list.push_back(effective_diam_axis.size());
  prf_lookup.load_data(prf_table, dim_list);

  // Link it all together
  table_set.add_table(prf_lookup);
  table_set.add_independent_variable( indep_mach);
  table_set.associate_table_and_independent();
  table_set.add_independent_variable( indep_alpha);
  table_set.associate_table_and_independent();
  table_set.add_independent_variable( indep_trail_diam);
  table_set.associate_table_and_independent();
  table_set.add_independent_variable( indep_eff_diam);
  table_set.associate_table_and_independent();

  table_set.initialize();
  if (table_set.is_initialized()) {
    // WakeEffectsBase::initialize doesn't exist, but it could in the future.
    // In the meantime, this will call SubscriptionBase::initialize().
    WakeEffectsBase::initialize();
    table_set.subscribe();
  }
}

/*****************************************************************************
update
Purpose:(Updates the model)
*****************************************************************************/
void
WakePrfModel::update()
{
  if (!active) {
    return;
  }

  generate_trail_and_radial_dist(); // and "in_region" flag.
                                    // In WakeEffectsBase

  trail_diameters = offset_trail_dist / main_body_diameter;
  // Calculate the reference diameter for B, in the wake */
  effective_diam = std::sqrt( 4 * effective_area_sf * objectB.get_area() / M_PI );

  if (in_region) {
    table_set.update();
    //PRF = PRF_mean + unc_factor * (1.0 - PRF_mean) * 0.4;
    PRF = PRF_mean + unc_factor * (1.0 - PRF_mean) * 0.4;


    if (objectB.get_generate_force()) {

      // NOTE - looks like RevFlow version but different constant and sign
      //        difference on axial_sep_vel, so keep independent.

      // FIXME Turner 2021/04
      //       PRF can be negative, and this uses the sqrt of PRF
      // eff_wake_vel = objectA.freestream_vel_mag * sqrt( PRF ) -
      //                objectB.axial_sep_vel;
      //       Going to try rearranging to apply PRF to dynamic-pressure
      //       instead, effectively modifying both the freestream
      //       velocity and separation velocity.
      eff_wake_vel = objectA.freestream_vel_mag - objectB.axial_sep_vel;
      force_toward_A = 0.5 * PRF * objectA.freestream_density *
                   eff_wake_vel * eff_wake_vel * objectB.get_drag_area();
      if (eff_wake_vel > 0) {
        force_toward_A *= -1;
      }
    }
  }
  else { // not in region.
    PRF = 1.0;
  }
}

/*****************************************************************************
deactivate
Purpose:(deactivates the model)
*****************************************************************************/
void
WakePrfModel::deactivate()
{
  PRF = 1.0;
  force_toward_A = 0;
  WakeEffectsBase::deactivate();
}
