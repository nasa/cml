/*******************************************************************************
PURPOSE:
   (Specific implementations of the WatchValuesBase base-class; these have
    JEOD dependencies.)

PROGRAMMERS:
   (((Gary Turner) (OSR) (August 2017) (Antares))
 ******************************************************************************/

#ifndef CML_WATCH_VALUES_SPECIFIC_JEOD_HH
#define CML_WATCH_VALUES_SPECIFIC_JEOD_HH

#include <list>
#include <utility> // std::pair
#include "jeod/models/environment/gravity/include/spherical_harmonics_gravity_controls.hh"

#include "watch_values_specific.hh" // for SWIG template definitions

/*****************************************************************************
WatchValuesGravityAdjust
Purpose:(Modifies the degree and order of a specified gravitational field)
NOTE: use of this->
*****************************************************************************/
template <typename varT>
class WatchValuesGravityAdjust : public WatchValuesBase<varT> {
 protected:
  jeod::SphericalHarmonicsGravityControls & grav_controls; /* (--)
       reference to the JEOD gravity controls being managed by this event.*/
  std::list< std::pair< varT, unsigned int> > grav_fidelity_list; /* (--)
       A list of paired triggers and targets.  Triggers are type varT template
       and might be, e.g., radial distance.  Targets are the values to which
       the spherical harmonics degree and order will be assigned; this event
       execution will assign degree = order.  For spherical gravity, use 0.*/
 public:
  WatchValuesGravityAdjust( jeod::SphericalHarmonicsGravityControls & grav_controls_)
    :
    WatchValuesBase<varT>(),
    grav_controls(grav_controls_)
  {
    // Make this class type default to being multi-shot.  This means that when
    // a crossing is detected, the event will not deactivate.  This behavior is
    // desirable in general because we have a list of events to process.
    // Shift the deactivation to be associated with the speecific-execution
    // method when the list of events has run out.
    this->multi_shot = true;
  }
  /***************************************************************************
  load_data populates grav_fidelity_list
  ***************************************************************************/
  void load_data( size_t size,
                  varT * thresholds,
                  unsigned int * deg_order)
  {
    for (size_t ii = 0; ii < size; ++ii) {
      std::pair< varT, unsigned int > new_pair( thresholds[ii], deg_order[ii]);
      grav_fidelity_list.push_back( new_pair);
    }
  }
 protected:
  /***************************************************************************
  specific_execution runs
  ***************************************************************************/
  virtual bool specific_execution()
  {
     grav_controls.degree =
     grav_controls.order  = grav_fidelity_list.front().second;
     grav_controls.spherical = (grav_controls.degree == 0);
     grav_fidelity_list.pop_front();

     // If that is the last setting, can stop watching.
     if (grav_fidelity_list.empty()) {
        this->active = false;
     }
     else {
        // Keep watch active, reset the reference value for the next scheduled
        // change
        this->reference = grav_fidelity_list.front().first;
     }
     return false;
  }
 private:
  WatchValuesGravityAdjust ( const WatchValuesGravityAdjust& rhs);
  WatchValuesGravityAdjust & operator = (
                                const WatchValuesGravityAdjust& rhs);
};
#endif
