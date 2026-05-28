/*******************************TRICK HEADER******************************
PURPOSE: (Provides a set of EventTriggers, analagous to how a
          WatchValuesSet provides a set of WatchValues.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (July 2023) (ANTARES)
    (Merging concepts found in former grok-events and CML-events))
  )
***********************************************************************/

#include <algorithm> // find_if
#include "../include/event_trigger_set.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
EventTriggerSet::EventTriggerSet()
{}


/*****************************************************************************
Destructor
*****************************************************************************/
EventTriggerSet::~EventTriggerSet()
{
  for ( WatchValuesBaseCore * trigger : allocated_triggers) {
    delete trigger;
  }
}


/*****************************************************************************
Name: populate_conditional_reference_trigger_list
Purpose:
  The EventTriggerSet maintains a list of triggers that have conditional
  reference values to facilitate the updating of those conditional reference
  values after the events have processed. This method adds a pointer to such
  a trigger to that list.
Note:
- All triggers have a reference value, aka threshold value; the value of the
  current variable is compared against this reference value to identify
  whether the trigger's comparison-logic is satisfied. Reference values can be:
  - fixed to a specific value for all time
  - continuously variable, with a value equal to that of another variable
  - conditionally variable, with a value that updates when conditions dictate;
    the updating of the reference value can be either:
    - conditional upon the satisfaction of the trigger's comparison logic
       (e.g. if the trigger is configured with comparison_logic = MAX, then
        the reference value should always be equal to maximum value previously
        encountered and should update whenever a new maximum is encountered.)
    - conditional upon the satisfaction of the EVENT's conditional logic
       (e.g. if the trigger is configured with comparison_logic =
        MAX_CONDITIONAL, then the reference value should always be equal to
        the maximum value previously encountered on a cycle during which the
        event was triggered. and should update only when a new maximum is
        encountered AND other circumstances resulted in satisfying the event's
        required conditions/triggers.)
  This list is for those triggers that fall into the last category, when the
  update to the reference value can onl be processed AFTER processing all of
  the event's triggers.
*****************************************************************************/
void
EventTriggerSet::populate_conditional_reference_trigger_list()
{
  for (WatchValuesBaseCore * trigger : triggers) {
    EventTriggerBase * event_trigger = dynamic_cast<EventTriggerBase*>(trigger);
    if (event_trigger != nullptr &&
        event_trigger->has_conditional_reference()) {
      conditional_reference_triggers.push_back(event_trigger);
    }
    // else ignore it, either not a conditional event trigger, or not an
    // event trigger at all.
  }
}

/*****************************************************************************
Name: update_conditional_trigger_references
Purpose:
  Updates the conditional triggers' reference values; called only if the
  entire event has been triggered.
Note:
  See description of populate_conditional_reference_trigger_list() method for
  an explanation of the concept of a conditional-trigger-reference.
*****************************************************************************/
void
EventTriggerSet::update_conditional_trigger_references()
{
  for (EventTriggerBase * trigger : conditional_reference_triggers) {
    trigger->set_new_reference();
  }
}


/*****************************************************************************
Name: find_trigger
Purpose:
  Returns a reference to a trigger stored in allocated_triggers
*****************************************************************************/
WatchValuesBaseCore *
EventTriggerSet::find_trigger( std::string name)
{
  auto it = std::find_if( allocated_triggers.begin(), allocated_triggers.end(),
                          [name](WatchValuesBaseCore* trigger_) {
                            return (trigger_->name == name);}
                        );
  if (it != allocated_triggers.end()) { return *it;}

  CMLMessage::error( __FILE__,__LINE__,
    "Could not find a trigger named ", name,".\nReturning NULL.\n");
  return nullptr;
}