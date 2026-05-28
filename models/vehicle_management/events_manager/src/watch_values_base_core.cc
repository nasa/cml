/*******************************************************************************
PURPOSE:
(Manages the watched variables)

PROGRAMMERS:
(((Gary Turner) (OSR) (October 2014) (Antares))

*******************************************************************************/

#include "../include/watch_values_base.hh"

/*****************************************************************************
Constructor
 ******************************************************************************/
WatchValuesBaseCore::WatchValuesBaseCore()
  :
  multi_shot(false),
  relative_to_activation(false),
  event_triggered(false),
  int_event_triggered(0),
  message(),
  add_self_to_manager_active_list(true),
  name(),
  locked(false),
  externally_managed(false),
  direction(Undefined),
  delta_record(0.0),
  relative_to_activation_protected(false),
  active_watches(0),
  ext_bool_on(0),
  ext_bool_off(0),
  subscribe_models(0),
  unsubscribe_models(0)
{
  subscribe_name = "WatchValuesBaseCore:";
}

/*****************************************************************************
initialize
Purpose:(Sets pointers)
******************************************************************************/
void
WatchValuesBaseCore::initialize(
       std::list<WatchValuesBaseCore *> * active_watch_in)
{
  // Single-shot method.  Don't want to be calling this multiple times, the
  // push_to_active_list should only go once.
  if (initialized) {
    return;
  }
  active_watches = active_watch_in;
  // NOTE - watches can be subscribed prior to initialization.  In that case,
  // they will be activated in the call to Subscriptionbase::initialize() below
  // Watches subscribed after initialization will be activated when they get
  // subscribed.
  // When a watch is activated, it gets added to the active_watches list.
  SubscriptionBase::initialize();
}

/*****************************************************************************
apply_complementary_changes
Purpose:(Applies flags to watches and models associated with a particular
         crossing.)
*****************************************************************************/
void
WatchValuesBaseCore::apply_complementary_changes()
{
  // activate complimentary watches
  // adjust exterior models
  for (auto & x : ext_bool_on) {
    *x = true;
  }
  for (auto & x : ext_bool_off) {
    *x = false;
  }
  for (auto & x : subscribe_models) {
    x->subscribe();
  }
  for (auto & x : unsubscribe_models) {
    x->unsubscribe();
  }
  for (auto & x : disable_models) {
    x->disable();
  }
  for (auto & x : assignments) {
    x->make_assignment();
  }
  for (auto & x : actions) {
    x->specific_execution();
  }
  if (!message.empty()) {
    CMLMessage::inform( __FILE__,__LINE__,
      "Event processed:\n", message, "\n\n");
  }
}

/*****************************************************************************
set_direction
Purpose:(Sets the direction.)
 ******************************************************************************/
void
WatchValuesBaseCore::set_direction( int arg)
{
  if (arg < 0) {
    direction = Decreasing;
  }
  else if ( arg > 0) {
    direction = Increasing;
  }
  else {
    direction = Both;
  }
}

/*****************************************************************************
Destructor
Purpose:()
*****************************************************************************/
WatchValuesBaseCore::~WatchValuesBaseCore()
{
  for (auto & assgt : assignments) {
    delete assgt;
  }
}
