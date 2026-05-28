/*******************************************************************************
PURPOSE:
   (Skeletal implementation of a template-based events management system to
    repalce the python-based Trick-events.)

ASSUMPTIONS AND LIMITATIONS:
 ((Assumption : This class is a skeletal implementation of an events manager.
       Any project using this implementation must identify the events to be
       managed in the project, and declare them in an inherited version of
       this class.)
  (Assumption : This is an abstract class; the intent is that this
       be used as a base-class, with a project-specific implementation being
       derived from it.)
 )

LIBRARY DEPENDENCIES:
   (../src/vehicle_events_manager.cc)

PROGRAMMERS:
   (((Gary Turner) (OSR) (October 2014) (Antares))
   )
 ******************************************************************************/

#ifndef CML_VEHICLE_EVENTS_MANAGER_HH
#define CML_VEHICLE_EVENTS_MANAGER_HH

#include<list>
#include "cml/models/utilities/trick_logging/include/trick_logging.hh"

#include "watch_values_base.hh"

/*****************************************************************************
VehicleEventsManager
Purpose:(Collection of all sets of watch-values.)
 ******************************************************************************/
class VehicleEventsManager
{
 public:
  // Instances of the events go here, e.g.
  // WatchValuesBase<int>              dummy_event; // (--) example
  // WatchValuesAdjustLogging<double>  logging_rate_change; /* (--) another

 protected:
  std::list<WatchValuesBaseCore *> active_watches;
  std::list<WatchValuesBaseCore *> all_watches;

 public:
  VehicleEventsManager();
  virtual ~VehicleEventsManager(){};

  virtual void initialize(); // Second part of init sequence, called late.
  virtual void update();
  virtual void execution_follow_up() = 0;

  void register_watch(WatchValuesBaseCore & watch) {all_watches.push_back(&watch);};

 private:
   VehicleEventsManager (const VehicleEventsManager& rhs);
   VehicleEventsManager& operator = (const VehicleEventsManager& rhs);
};

#endif
