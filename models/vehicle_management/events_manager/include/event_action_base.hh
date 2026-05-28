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
#ifndef CML_EVENT_ACTION_BASE_HH
#define CML_EVENT_ACTION_BASE_HH

/*****************************************************************************
EventActionBase
Purpose:
  The default EventAction, used by simple events and legacy events that
  were previously defined by extension of WatchValuesBase. It does nothing.
*****************************************************************************/
class EventActionBase {
 public:
  virtual bool specific_execution() { return false;}
  virtual ~EventActionBase(){};
};
#endif
