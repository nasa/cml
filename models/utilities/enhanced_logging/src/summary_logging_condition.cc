/*******************************TRICK HEADER******************************
PURPOSE: (
  A EnhancedLogging_SummaryCondition represents an extension to the concept of
  a compound-event -- see CML compound-event model.
  The event identifies whether the arming and action conditions have been
  satisfied.
  This extension provides additional information on how the logging variables
  should proceed when the event conditions have been satisfied.

  For example, when altitude < ceiling, record the maximum temperature
  and pressure:
    - the event would monitor the event-condition, altitude < ceiling
    - this extension would identify that the maximum value is to be recorded
      for as long as that event-condition is satsified.
    - a summary-logging-group would add the variables, pressure and temperature
      and a condition monitoring the event with a MAX configuration.
    - The two SummaryLoggingVariables will have a data slot to record their
      values in response to this condition.
    - At execution, the condition will be updated, which will evaluate the
      event: recognizing that altitude < ceiling, the event will be marked
      as triggered, then the variables will compare their current values
      against the recorded values in their memory slot associated with this
      condition. If the current values exceed the recorded values, the
      recorded values will be overwritten.
  )

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES) (new)))
**********************************************************************/

#include "../include/summary_logging_condition.hh"


/*****************************************************************************
Constructors
*****************************************************************************/
EnhancedLogging_SummaryCondition_GroupData::EnhancedLogging_SummaryCondition_GroupData (
  EnhancedLogging_SummaryConditionBase & condition_,
  unsigned int ID_)
  :
  condition(&condition_),
  first_time(0.0),
  last_time(0.0),
  count(0),
  ID(ID_),
  active(true)
{}
/****************************************************************************/
EnhancedLogging_SummaryConditionBase::EnhancedLogging_SummaryConditionBase()
  :
  record_type(SINGLE),
  initialized(false),
  name("Unnamed")
{}
/****************************************************************************/
EnhancedLogging_SummaryConditionBool::EnhancedLogging_SummaryConditionBool(
  const bool & event_triggered_)
  :
  EnhancedLogging_SummaryConditionBase(),
  event_triggered( event_triggered_)
{}
/****************************************************************************/
EnhancedLogging_SummaryConditionEvent::EnhancedLogging_SummaryConditionEvent(
  const double & dyn_time)
  :
  EnhancedLogging_SummaryConditionBase(),
  event(dyn_time)
{
  event.add_self_to_manager_active_list = false;
}
/****************************************************************************/
EnhancedLogging_SummaryConditionEventRef::EnhancedLogging_SummaryConditionEventRef(
  CompoundEvent & event_)
  :
  EnhancedLogging_SummaryConditionBase(),
  event(event_)
{}


/*****************************************************************************
initialize
Purpose:
  Pass-through to initialize the EnhancedLogging_SummaryCondition* object.
*****************************************************************************/
void
EnhancedLogging_SummaryCondition_GroupData::initialize()
{
  condition->initialize();
}
/****************************************************************************/
void
EnhancedLogging_SummaryConditionBase::initialize()
// Note: also EnhancedLogging_SummaryConditionBool::initialize()
{
  initialized = true;
}
/****************************************************************************/
void
EnhancedLogging_SummaryConditionEvent::initialize()
{
  /* Note: event has no "if (!initialized)" condition because if the event
    logic is updated, it needs to be possible to re-initialize it.*/
  event.initialize(nullptr);
  if (!initialized) {
    event.subscribe();
  }
  initialized = true;
}
/****************************************************************************/
void
EnhancedLogging_SummaryConditionEventRef::initialize()
{
  if (!initialized) {
    event.subscribe();
  }
  initialized = true;
}


/*****************************************************************************
update
Purpose:
  Access point from SummaryLoggingGroup to update the
  EnhancedLogging_SummaryCondition instances registered to the group.
Notes:
 - The activity of a condition is a group-specific setting, so is evaluated in
   the EnhancedLogging_SummaryCondition_GroupData interface; if the group is
   actively using the condition, then query the condition.
 - Baseline condition is always satisfied.
 - Bool-based conditions are satisfied if and only if the bool flag is true.
 - Event-based conditions query the event and are satisfied if the event is
   triggered.
*****************************************************************************/
bool
EnhancedLogging_SummaryCondition_GroupData::update( double time)
{
  if (!active) {return false;}

  if (condition->update()) {
    last_time = time;
    if (count == 0) {
      first_time = time;
      if (condition->record_type ==
                               EnhancedLogging_SummaryConditionBase::SINGLE) {
        active = false;
      }
    }
    count++;
    return true;
  }
  return false;
}
/****************************************************************************/
bool
EnhancedLogging_SummaryConditionBase::update()
{
  return true;
}
/****************************************************************************/
bool
EnhancedLogging_SummaryConditionBool::update()
{
  return event_triggered;
}
/****************************************************************************/
bool
EnhancedLogging_SummaryConditionEvent::update()
{
  if (event.test_crossing()) {
    event.apply_complementary_changes();
  }
  return event.event_triggered;
}
/****************************************************************************/
bool
EnhancedLogging_SummaryConditionEventRef::update()
{
  if (event.test_crossing()) {
    event.apply_complementary_changes();
  }
  return event.event_triggered;
}
