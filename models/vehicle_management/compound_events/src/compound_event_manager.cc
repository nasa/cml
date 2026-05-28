/********************************* TRICK HEADER *******************************

PURPOSE:
   (Define method to check events for all Compound-event instances)

PROGRAMMERS:
   (
    ((Jeremy Rea) (NASA) (May       2018) (Initial implementation of event manager))
    ((Jeremy Rea) (NASA) (June      2018) (Add capability to set separate arming, action, and disarming call rates))
    ((Jeremy Rea) (NASA) (June      2018) (Add capability to use trick.stop and trick.message_publish as actions))
    ((Jeremy Rea) (NASA) (July      2018) (Add capability to write data collection file as an event action))
    ((Gary Turner) (OSR) (July 2023) (ANTARES)
    (Merging concepts found in former grok-events and CML-events))
   )

*******************************************************************************/


/* Model Includes */
#include "../include/compound_event_manager.hh"


/*****************************************************************************
Constructor / Destructor
*****************************************************************************/
CompoundEventsManager::CompoundEventsManager(
  const double & time_)
  :
  time(time_),
  allocated_events(),
  managed_triggers(),
  allocated_triggers(),
  allocated_triggers_(),
  enabled(true)
{}
/****************************************************************************/
CompoundEventsManager::~CompoundEventsManager()
{
  for ( WatchValuesBaseCore * event : allocated_events) {
    delete event;
  }
  for ( WatchValuesBaseCore * trigger : allocated_triggers) {
    delete trigger;
  }
}

/*****************************************************************************
initialize
Purpose:
  Initialize all the events and their triggers as configured at
  sim-initialization.
*****************************************************************************/
void
CompoundEventsManager::initialize()
{
  if (!enabled) {return;}

  VehicleEventsManager::initialize();
}

/*****************************************************************************
update
Purpose: Main execution
*****************************************************************************/
void
CompoundEventsManager::update()
{
  if (!enabled) {return;}
  // Iterate through managed triggers and unlock them for the current sim cycle
  // (meant to be executed before a trigger is updated, where it will then be locked)
  for ( WatchValuesBaseCore * trigger : managed_triggers) {
    trigger->locked = false;
  }
  VehicleEventsManager::update();
}

/*****************************************************************************
add_event
Purpose:
  Adds a CompoundEvent (or any WatchValuesBaseCore) to the Manager's list
  by pushing it up to the VehicleEventsManager, which does most of the work
  behind the scenes.
*****************************************************************************/
void
CompoundEventsManager::add_event( WatchValuesBaseCore & event)
{
  register_watch( event);
}

/*****************************************************************************
create_event
Purpose:
  An input-file-facing creation tool to build a new event from an input file.
  This tool works better from a JIT input file.
*****************************************************************************/
CompoundEvent &
CompoundEventsManager::create_event()
{
  CompoundEvent * new_event = new CompoundEvent(time);
  allocated_events.push_back( new_event);
  add_event(*new_event);
  return *new_event;
}

/*****************************************************************************
add_trigger
Purpose:
  Adds a trigger to the list of managed_triggers, which will enable locking
  and unlocking of the trigger.
*****************************************************************************/
void
CompoundEventsManager::add_trigger(WatchValuesBaseCore* new_trigger)
{
  managed_triggers.push_back( new_trigger);
  new_trigger->externally_managed = true;
}

/*****************************************************************************
Name: get_trigger
Purpose:
  Returns a trigger based on its name.
*****************************************************************************/
WatchValuesBaseCore *
CompoundEventsManager::get_trigger(std::string name)
{
  auto it = std::find_if( managed_triggers.begin(), managed_triggers.end(),
                          [name](WatchValuesBaseCore * trigger_) {
                            return (trigger_->name == name); }
                        );
  if (it == managed_triggers.end()) {
    CMLMessage::error(__FILE__,__LINE__,
      "Manager has does not manage a trigger with name ",name,".\n"
      "Returning NULL.\n");
    return nullptr;
  }
  return (*it);
}

/*****************************************************************************
Name: set_trigger_dbl_reference
Purpose:
  Sets the value of the trigger's trigger-point.
  Trigger can be specified as an argument or by name
*****************************************************************************/
void
CompoundEventsManager::set_trigger_dbl_reference(
  WatchValuesBaseCore & trigger,
  double ref)
{
  trigger.set_dbl_reference(ref);
}
/****************************************************************************/
void
CompoundEventsManager::set_trigger_dbl_reference(
  std::string name,
  double ref)
{
  WatchValuesBaseCore * trigger = CompoundEventsManager::get_trigger(name);
  if (trigger) {
    CompoundEventsManager::set_trigger_dbl_reference( *trigger,
                                                      ref);
  }
}

// void
// CompoundEventsManager::set_trigger_comparison_logic(WatchValuesBaseCore * trigger, double ref)
// {
//   trigger->set_comparison_logic(ref);
// }

// void
// CompoundEventsManager::set_trigger_comparison_logic(std::string name, double ref)
// {
//   WatchValuesBaseCore * trigger = CompoundEventsManager::get_trigger(name);
//   CompoundEventsManager::set_trigger_comparison_logic(trigger,ref);
// }

/*****************************************************************************
Name: set_trigger_delay_offset
Purpose:
  sets the trigger delay for a specified trigger.
  Trigger can be specified as an argument or by name
*****************************************************************************/
void
CompoundEventsManager::set_trigger_delay_offset(
  WatchValuesBaseCore & trigger,
  double ref)
{
  trigger.set_delay_offset(ref);
}
/****************************************************************************/
void
CompoundEventsManager::set_trigger_delay_offset(
  std::string name,
  double ref)
{
  WatchValuesBaseCore * trigger = CompoundEventsManager::get_trigger(name);
  if (trigger) {
    CompoundEventsManager::set_trigger_delay_offset( *trigger,
                                                     ref);
  }
}
