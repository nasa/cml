/*******************************************************************************
PURPOSE:
(Manages the declared events and watched variables)

PROGRAMMERS:
(((Gary Turner) (OSR) (October 2014) (Antares))
)
*******************************************************************************/

#include "../include/vehicle_events_manager.hh"

/*****************************************************************************
Constructor
******************************************************************************/
VehicleEventsManager::VehicleEventsManager()
   // :
   // construct each of the event instances here, e.g.:
   // dummy_event()
   // another( trick_logging)  // need a ref to trick_logging to use this
{}

/*****************************************************************************
initialize
Purpose:(initializes all of the watches)
******************************************************************************/
void
VehicleEventsManager::initialize()
{
  for (auto & x :all_watches) {
    if (x->is_enabled()) {
      // Note - the individual watches need to be able to see active_watches
      // so that they can add themselves as they get activated.  Hence the
      // inclusion of the active_watches argument, so that the watch can seta
      // pointer to it.
      x->initialize( &active_watches);
    }
  }
  // NOTE - Do not run update() here.  There is a chance that something is not
  //        yet initialized, and there could be a falsely triggered event.
  //        Additionally, update() gets run as one of the very first scheduled
  //        executions, so it is coming up real soon....
  return;
}

/*****************************************************************************
update
Purpose:(Checks all active watches)
******************************************************************************/
void
VehicleEventsManager::update()
{
  std::list<WatchValuesBaseCore *>::iterator ii = active_watches.begin();
  while (ii != active_watches.end()) {
    // If the crossing is detected
    if ( (*ii)->test_crossing()) {
      // run the specific execution.  This returns a boolean value
      // which can be used to trigger additional capabilities
      if ( (*ii)->specific_execution()) {
        execution_follow_up();
      }
      // Turn on other watches and model modifications:
      (*ii)->apply_complementary_changes();
    }
    // If no longer active, erase from the list.
    // Continue the iteration in the same location, which will be the next
    // element moved 1 place to the left.
    if ( !(*ii)->is_active()) {
      ii = active_watches.erase(ii);
    }
    else {
      ++ii;
    }
  }
  return;
}
