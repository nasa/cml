/*******************************TRICK HEADER******************************
PURPOSE: (Provides a set of EventTriggers, analagous to how a
          WatchValuesSet provides a set of WatchValues.)

LIBRARY DEPENDENCY:
   ((../src/event_trigger_set.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (July 2023) (ANTARES)
    (Merging concepts found in former grok-events and CML-events))
  )
***********************************************************************/
#ifndef CML_EVENT_MANAGEMENT_EVENT_TRIGGER_SET_HH
#define CML_EVENT_MANAGEMENT_EVENT_TRIGGER_SET_HH

#include <list>
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/vehicle_management/events_manager/include/watch_values_set.hh"
#include "event_trigger.hh"
/*****************************************************************************
EventTriggerSet
Purpose:
  A specialized type of WatchValuesSet that allows for querying the
  comparison logic on its associated triggers.
  Some comparison logic implementations require the trigger references be
  updated conditionally upon the whole trigger-set passing.
  This is only relevant to the use of EventTrigger extensions of WatchValues,
  so it makes no sense to implement this at the WatchValuesSet level (which
  can only use WatchValues).
*****************************************************************************/
class EventTriggerSet : public WatchValuesSet
{
 protected:
  std::list<WatchValuesBaseCore*>  allocated_triggers; /* (--)
    Set of triggers allocated for unique use within this trigger-set.
    This trigger-set may also use triggers allocated elsewhere; this is
    the list of triggers for which this EventTriggerSet instance has
    memory-management responsibilities.*/
 public:
  std::list< EventTriggerBase*> conditional_reference_triggers; /* (--)
    A subset of the triggers found in the triggers vector (inherited from
    WatchValuesSet) that have the necessity of maintaining the reference
    value subject to the condition that the CompoundEvent is triggered. */

  EventTriggerSet();
  virtual ~EventTriggerSet();

  void populate_conditional_reference_trigger_list();
  void update_conditional_trigger_references();
  WatchValuesBaseCore * find_trigger( std::string name);


 private: // not implemented / deleted
  EventTriggerSet( const EventTriggerSet&);
  EventTriggerSet& operator=( const EventTriggerSet&);
};
#endif
