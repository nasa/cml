/*******************************TRICK HEADER******************************
PURPOSE: (Provides a multi-conditional event structure.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2023) (Antares) (enhancement)))
**********************************************************************/

#include <algorithm> // any_of
#include "../include/watch_values_set.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
WatchValuesSet::WatchValuesSet()
  :
  triggers(),
  require_all(false),
  record_trigger_name(false)
{}

/*****************************************************************************
add_trigger
Purpose:
  Adds a trigger to the set. This is the only way in which the triggers
  vector can be modified.
*****************************************************************************/
void
WatchValuesSet::add_trigger( WatchValuesBaseCore & new_trigger)
{
  if (initialized) {
    CMLMessage::error( __FILE__,__LINE__,
      "Cannot add a new trigger to an initialized WatchValuesSet.\n");
    return;
  }
  triggers.push_back( &new_trigger);
}

/*****************************************************************************
intialize
Purpose:
  Because the individual elements in triggers are not loaded directly
  onto the manager, this class must call their respective initializations )
*****************************************************************************/
void
WatchValuesSet::initialize( std::list<WatchValuesBaseCore *> * active_watch_in)
{
  if (triggers.empty()) {
    CMLMessage::error( __FILE__,__LINE__,
     "Cannot initialize a WatchValuesSet without any associated triggers.\n");
    return;
  }
  for (WatchValuesBaseCore * trigger : triggers) {
    trigger->add_self_to_manager_active_list = false;
    trigger->SubscriptionBase::initialize();
  }
  WatchValuesBaseCore::initialize( active_watch_in);
}


/*****************************************************************************
test_crossing
Purpose:(The logic to execute the individual test_crossing implementations
         and combine to give the overall result.)
*****************************************************************************/
bool
WatchValuesSet::test_crossing()
{
  if (!active) {return false;}

  for (WatchValuesBaseCore * trigger : triggers) {
  /* Check each (active) trigger.
     Note -- test_crossing() sets the trigger's event_triggered variable.*/
    if (trigger->test_crossing()) {
      trigger->apply_complementary_changes();
    }
  }

  /* If the set triggers with ANY trigger, and any trigger satisfies its
     condition, the set is Triggered.
     If the set only triggers with ALL triggers, and any trigger does not
     satisfy its condition, the set is NotTriggered.
     So we can compare the require_all boolean against the event_triggered
     value of each trigger-event to identify any significant single results.
     - If any trigger has a status that does not match with "require_all", then
       the overall status is "not require_all"
     - Conversely, if all triggers have status matching "require_all", then the
       overall status is going to be "require_all"*/
  event_triggered = std::any_of( triggers.begin(), triggers.end(),
                      [this](WatchValuesBaseCore * trigger_) {
                        return (require_all != trigger_->event_triggered);} )?
                    !require_all :
                    require_all;

  if (event_triggered && !multi_shot) {
    deactivate();
  }

  if (record_trigger_name) {
    if (event_triggered) {
      for (WatchValuesBaseCore * trigger : triggers) {
        if (trigger->event_triggered) {
          trigger_name = trigger->name;
        }
        break;
      }
    } else {
      trigger_name = "";
    }
  }

  int_event_triggered = event_triggered ? 1 : 0;
  return event_triggered;
}

/*****************************************************************************
activate
Purpose:
  Called when the event gets subscribed; need to activate all of the trigger
  events as well so that their test_crossing methods can execute.
  Note that the manager's active_watch list will get this class added to it,
  but not the individual associated triggers.
*****************************************************************************/
void
WatchValuesSet::activate()
{
  for (WatchValuesBaseCore * trigger : triggers) {
    trigger->subscribe();
  }
  active = true;
  if (add_self_to_manager_active_list) {
    if (!active_watches) {
      CMLMessage::error( __FILE__,__LINE__,
        "Activating watch with internal instruction to add itself to\n"
        "the active-watch list, but has no access to that list.\n"
        "WatchValueSet will not get checked by the event-manager.\n");
    } else {
      active_watches->push_back(this);
    }
  }
}

/*****************************************************************************
deactivate
Purpose:
  Unsubscribe from the triggers to deactivate them.
  This is important so that a subsequent activation will re-activate them,
  enabling the "relative to activation" behavior.
  Do not need to be concerned about removing this instance from the
  manager's active_watch list; it will remove it on the next pass when
  it identifies this watch as being inactive.
  The triggers watches were never loaded onto the manager so no
  action needed there either.
*****************************************************************************/
void
WatchValuesSet::deactivate()
{
  for (WatchValuesBaseCore * trigger : triggers) {
    trigger->unsubscribe();
  }
  active = false;
}
