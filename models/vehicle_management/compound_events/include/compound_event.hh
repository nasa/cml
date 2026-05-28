/*******************************TRICK HEADER******************************
PURPOSE: (Provide compound event triggers)

LIBRARY DEPENDENCY:
   ((../src/compound_event.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (July 2023) (ANTARES)
    (Merging concepts found in former grok-events and CML-events))
  )
***********************************************************************/
#ifndef CML_EVENT_MANAGEMENT_COMPOUND_EVENTS_HH
#define CML_EVENT_MANAGEMENT_COMPOUND_EVENTS_HH

#include <list>
#include "cml/models/vehicle_management/events_manager/include/watch_values_delay.hh"
#include "cml/models/vehicle_management/events_manager/include/watch_values_set.hh"

#include "event_trigger_set.hh"

// Provide SWIG with the necessary template so it can see through to the
// layer above.
#ifdef SWIG
#ifndef CML_COMPOUND_EVENTS_SWIG_TEMPLATES
#define CML_COMPOUND_EVENTS_SWIG_TEMPLATES
%template(WatchValuesDelay_bool) WatchValuesDelay<bool>;
#endif
#endif

/*****************************************************************************
CompoundEvent
Purpose:
  Represents a more complex event sequence than a simple WatchValues or
  even a WatchValuesSet.
  This uses EventTriggers as specialized types of WatchValues, collected
  into groups (WatchValuesSet). There are 3 such groups:
  - one to arm the event,  (optional)
  - one to disarm the event (optional)
  - one to implement the action.

  In its simplest form, with no arming triggers, no disarming triggers, and
  1 simple action trigger that could be represented as a simple WatchEvent,
  this whole system collapses down onto that single WatchEvent, and when
  it triggers, this event triggers.
  But it can get much more complex than that.
*****************************************************************************/
class CompoundEvent : public WatchValuesDelay<bool>
{
 protected:
  const double & time; /* (s)
    Reference to sim dynamic-time. Note that this may be redundant
    with the class's inherited reference delay_ref if the delay parameter
    is also time.  However, that is not guaranteed and time is needed
    for the time-delay between arming/disarming/action checks.*/

 public:
  enum CompoundEventStatus
  {
    Unarmed       = 0,
    Armed         = 1,
    ArmedNoAction = 2, // Used for arm/disarm, no-action events.
    HoldAction    = 3,
    PostAction    = 4
  };
  std::string name; /* (--) Optional name for debugging purposes.*/
  EventTriggerSet arming_triggers; /* (--)
    Set of triggers for arming the event.*/
  EventTriggerSet disarming_triggers; /* (--)
    Set of triggers for disarming the event.*/
  EventTriggerSet action_triggers; /* (--)
    Set of triggers for causing the event to trigger the event and cause
    it to issue its actions.*/
  bool allow_immediate_actions; /* (--)
    Flag to indicate whether to allow immediate disarming or immediate actions
    following the arming of an event.
    Default: false (follow-on triggers will not be tested until next cycle)*/
  bool deactivate_at_disarm; /* (--)
    Flag to deactivate the event immediately after being disarmed.*/
  double arm_trigger_check_period; /* (s)
    Time between adjacent checks of the arming triggers. */
  double disarm_trigger_check_period; /* (s)
    Time between adjacent checks of the disarming triggers. */
  double action_trigger_check_period; /* (s)
    Time between adjacent checks of the action triggers. */

 protected:
  bool event_has_action_triggers;/* (--)
    Flag indicating that the trigger-set action_triggers contains at least
    one trigger.
    Default: false.*/
  double time_triggered; /* (s)
    Value of time at which the event was triggered --
    i.e. reached its action phase.*/
  double next_arming_check; /* (s)
    Time at which next arming check is to be conducted.*/
  double next_disarming_check; /* (s)
    Time at which next disarming check is to be conducted.*/
  double next_action_check; /* (s)
    Time at which next action check is to be conducted.*/
  double discrete_time_elapsed; /* (s)
    Elapsed time since the event triggered.*/
  unsigned int number_of_times_armed; /* (1)
    Count of the number of times the event has been armed.*/

  CompoundEventStatus status;

  // Methods
 public:
  explicit CompoundEvent( const double & time);
  virtual ~CompoundEvent(){};

  void initialize( std::list<WatchValuesBaseCore *> * active_watches);
  bool test_crossing();
  void test_crossing_iterative();

  // For Python input file support:
  WatchValuesBaseCore & get_event_core() {return *this;}

  bool update_no_manager();

 protected:
  void activate();
//  void check_arming_trigger();

 private: // not implemented / deleted
  CompoundEvent( const CompoundEvent&);
  CompoundEvent& operator=( const CompoundEvent&);
};
#endif
