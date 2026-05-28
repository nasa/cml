/*******************************TRICK HEADER**********************************
PURPOSE: (Uses JEOD radiation pressure model components to generate an
          illumination fraction without executing the whole radiation
          pressure model.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (August 2019) (Antares) (initial)))
*****************************************************************************/

#include "../include/eclipse_calculator.hh"
#include "jeod/models/utils/math/include/vector3.hh" // vmag

/*****************************************************************************
Constructor
*****************************************************************************/
EclipseCalculator::EclipseCalculator(
        const double & time_,
        jeod::DynManager & dyn_manager_,
        jeod::RefFrame & vehicle_frame_)
  :
  time(time_),
  dyn_manager(dyn_manager_),
  vehicle_frame(vehicle_frame_),
  earth_name{"Earth"},
  moon_name{"Moon"},
  sun(),
  earth(),
  moon(),
  illum_factor(1.0)
{
  earth.primary_source_ptr = &sun;
  moon.primary_source_ptr = &sun;
  earth.name = earth_name;
  moon.name = moon_name;
  earth.active = false;
  moon.active = false;
}


/*****************************************************************************
initialize
Purpose:(Checks the configuration and initializes the model and its
         components.)
NOTE - must be called AFTER the dynamics-manager / ephem-manager has had a
       chance to build the ephemeris files because activate() executes the
       first call to update, and upate requires the ephemeris tree.
       Call with phase-priority P_DYN is acceptable.
*****************************************************************************/
void
EclipseCalculator::initialize()
{
  sun.initialize(&dyn_manager);
  earth.initialize(&dyn_manager);
  moon.initialize(&dyn_manager);
  SubscriptionBase::initialize();
}

/*****************************************************************************
update
Purpose:(Routine update of the model to recompute the illumination factor)
*****************************************************************************/
void
EclipseCalculator::update()
{
  if (!active) {
    return;
  }

  // In the full radiation-pressure model, these two calculations are
  // performed as part of the routine update in generating the solar flux at
  // the vehicle's distance.  In this partial model the flux calculation is
  // not performed, but these values are still needed for computing the
  // eclipsing effects.
  vehicle_frame.compute_position_from( *(sun.inertial_frame_ptr),
                                       sun.source_to_cg);
  sun.d_source_to_cg = jeod::Vector3::vmag( sun.source_to_cg);

  illum_factor = 1.0;
  if (earth.active) {
    illum_factor *= earth.process_third_body( time,
                                              vehicle_frame);
  }
  if (moon.active) {
    illum_factor *= moon.process_third_body( time,
                                             vehicle_frame);
  }
}

/*****************************************************************************
activate
Purpose:(called when the first subscription is submitted)
*****************************************************************************/
void
EclipseCalculator::activate()
{
  // Need the sun-inertial and earth-pfix and moon-pfix frames to be active
  // for computing the state of the planet wrt the sun (aka source) and
  // the consequential state of the vehicle wrt the planet.
  earth.active = true;
  moon.active = true;
  // NOTE - these three pointers have all been checked for non-NULL in
  // RadiationThirdBody::initialize(...)
  // Because EclipseCalculator inherits from SubscriptionBase, this method can
  // only be called after this class has been initialized, which means
  // RadiationThirdBody has also been initialized, which means these pointers
  // should be valid.
  sun.inertial_frame_ptr->subscribe();
  earth.local_frame_ptr->subscribe();
  moon.local_frame_ptr->subscribe();
  // Before updating, rebuild the ephemeris tree just to make sure everything
  // is connected properly.
  dyn_manager.update_ephemerides();

  SubscriptionBase::activate();
  update();
}

/*****************************************************************************
deactivate
Purpose:(called when the last unsubscribe() is submitted.)
*****************************************************************************/
void
EclipseCalculator::deactivate()
{
  illum_factor = 1.0;
  SubscriptionBase::deactivate();
  sun.inertial_frame_ptr->unsubscribe();
  earth.local_frame_ptr->unsubscribe();
  moon.local_frame_ptr->unsubscribe();
}
