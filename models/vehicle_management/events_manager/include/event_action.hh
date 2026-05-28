/*******************************TRICK HEADER******************************
PURPOSE: (
  The concept of Event Actions was originally incorporated intrinsically
  into the WatchValues* classes, with the intention of specifying a new
  type of WatchValue (i.e. event) whenever an event was needed to perform
  a new specific action, and those actions would be defined in the
  specific_execution() method of that event.

  Two patterns emerged during the ongoing usage of events management:
  - that some events needed multiple actions
  - that more complex triggering capabilities were needed

  Using the original paradigm, when an event needed to perform multiple actions,
  a new event would have to be defined and all actions defined in its
  specific_execution method, which resulted in duplicated code.

  As more triggering capabilities were added to the model to extend the
  capabilities of WatchValuesBase (e.g WatchValuesBase2, WatchValuesDelay,
  WatchValuesSet), it was recognized that extending WatchValuesBase in
  2 directions (one for the actions and one for the triggering) was
  also problematic. Many of the specialized actions defined for simple triggering
  would also be useful for the events using enhanced triggering capabilities,
  but would require a new (redundant) implementation of the exact same
  action-capability to extend one of the triggering-extended classes
  (that, or get diamond inheritance).

  So the EventAction concept was added to the WatchValuesBaseCore class.

  However, that means that the configuration of the event-actions is now
  encapsulated in a separate entity, whereas it was directly configured in the
  event structure when actions were defined by inheriting from WatchValuesBase.
  This breaks backward compatibility.

  So the old specific watch-values classes are retained, and those capabilities
  duplicated into EventActions. These event actions can be instantiated
  independently and added to any instance of WatchValuesBase, including any of
  the triggering extensions of WatchValuesBaseCore, or any legacy action-driven
  extensions of WatchValuesBase.
  )

PROGRAMMERS:
  (((Gary Turner) (OSR) (date) (project) (comment)))
**********************************************************************/
#ifndef CML_EVENT_ACTIONS_HH
#define CML_EVENT_ACTIONS_HH

#include <list>
#include <string>
#include "trick/exec_proto.h"
#include "cml/models/utilities/trick_logging/include/trick_logging.hh"

#include "event_action_base.hh"

/*****************************************************************************
EventActionSimStopNow
Purpose:
 Stops the sim
*****************************************************************************/
class EventActionSimStopNow : public EventActionBase {
 public:
  bool specific_execution() {
    exec_set_terminate_time(exec_get_sim_time());
    return false;}
};

/*****************************************************************************
EventActionSimStopDyn
Purpose:
  Monitors a list of the "translational_dynamics" flags of all the vehicles
  in the sim, and shuts the sim down when all vehicles are finished.
  It requires that the flags be loaded onto this event's list, trans_dyn_flags.
*****************************************************************************/
class EventActionSimStopDyn : public EventActionBase {
 public:
  std::list<bool*> trans_dyn_flags;
  bool specific_execution() {
    // once we delete ii from the list we can no longer increment it.
    // so we do some gymnastics to avoid that problem: get a copy of the
    // to-be-deleted item's pointer, move on to the next item, then delete the
    // item behind it using the copied pointer.
    for(std::list<bool*>::iterator ii = trans_dyn_flags.begin();
                                   ii != trans_dyn_flags.end(); ) {
       std::list<bool*>::iterator to_parse = ii;
       ++ii;
       if(!(**to_parse)) {
          trans_dyn_flags.erase(to_parse);
       }
    }
    if(trans_dyn_flags.empty()) {
       exec_set_terminate_time(exec_get_sim_time());
    }
    return false;
  }
};

/*****************************************************************************
EventActionDeactivateSimObject
Purpose: Turns off all jobs in a sim object
*****************************************************************************/
class EventActionDeactivateSimObject : public EventActionBase {
 public:
   std::string simobj_name;
   bool specific_execution() {
     exec_set_sim_object_onoff(simobj_name.c_str(), 0);
     return false;};
};

/*****************************************************************************
EventActionAdjustLogging
Purpose: Adjust logging rate across all logging groups
*****************************************************************************/
class EventActionAdjustLogging : public EventActionBase {
 protected:
  TrickLogging & logging;
 public:
  double log_cycle; /* (s) New logging frequency */
  bool   log_now;   /* (--) log and reset the logging schedule immediately.*/
  EventActionAdjustLogging(TrickLogging & logging_in)
    :
    logging(logging_in),
    log_cycle(1.0),
    log_now(false)
  {};
  bool specific_execution() {
    logging.set_cycle(log_cycle);
    if (log_now) {
      logging.reset_next_call_time();
    }
    return false;
  }
 private:
  EventActionAdjustLogging (const EventActionAdjustLogging&);
  EventActionAdjustLogging& operator = (const EventActionAdjustLogging&);
};

/*****************************************************************************
EventActionAdjustLoggingGroup
Purpose:
  Adjust logging rate and enable/disable a specific group or collection of
  groups.
*****************************************************************************/
class EventActionAdjustLoggingGroup : public EventActionBase {
 protected:
  TrickLogging & logging;
 public:
  std::list< std::string> group_name_list; /* (--)
    List of group-names to be processed by this action. If there iss only 1
    name, just use "group_name".*/
  std::string group_name; /* (--)
    If this is used for a specific group, this is the group name.
    If this is left empty, the intruction will be applied to all Trick
    Logging groups.*/
  bool enable_group; /* (--)
    This flag is used to enable and disable the group.
    Note -- this flag can be set as part of the add_ext_bool_on/off within the
    same event that executes this action.
    Default: true (group is enabled).*/
  double log_cycle; /* (s) New logging frequency */
  bool   log_now;   /* (--) log and reset the logging schedule immediately.*/
  EventActionAdjustLoggingGroup(TrickLogging & logging_in)
    :
    logging(logging_in),
    group_name(),
    enable_group(true),
    log_cycle(1.0)
  {};

  bool specific_execution() {
    if (group_name_list.empty()) {
      process_group( group_name);
    } else {
      for (std::string & name : group_name_list) {
        process_group (name);
      }
    }
    return false;
  }

  void process_group( const std::string & group_name_)
  {
    Trick::DataRecordGroup * drg = logging.get_group( group_name_);
    if (drg) {
      if (enable_group) {drg->enable();}
      else {drg->disable();}
      drg->set_cycle(log_cycle);
      logging.reset_next_call_time(*drg);
    }
  }
 private:
  EventActionAdjustLoggingGroup (const EventActionAdjustLoggingGroup&) = delete;
  EventActionAdjustLoggingGroup& operator = (
                                 const EventActionAdjustLoggingGroup&) = delete;
};

/*****************************************************************************
EventActionLogNow
Purpose:
  Forces logging to occur at this point in the sim without modifying
  the logging rates
*****************************************************************************/
class EventActionLogNow : public EventActionBase {
 protected:
  TrickLogging & logging;
 public:
  EventActionLogNow(TrickLogging & logging_in)
     :
     logging(logging_in)
  {};
  bool specific_execution() {
    logging.log_now();
    return false;
  }
 private:
  EventActionLogNow (const EventActionLogNow& rhs);
  EventActionLogNow& operator = (const EventActionLogNow& rhs);
};
#endif
