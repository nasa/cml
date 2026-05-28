/********************************* TRICK HEADER ********************************
PURPOSE: (
  A special type of Watch-value that monitors some associated watch-value.
  This flip-flop type maintains a boolean state that reflects whether the
  associated watch-value has its conditions satisfied.
  The flip-flop type is "triggered" only by a change in this boolean state.
  The basic watch-values has a set of actions that are applied when its
  conditions are satisfied. This type has two sets:
  - one is applied when the boolean state switches from false to true
  - the other is applies when the state switches from true to false.
 )
PROGRAMMERS:
  (((Gary Turner) (OSR) (2026/02) (ANTARES) (initial implementation)))
*******************************************************************************/
#include "../include/watch_values_flipflop.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
WatchValuesFlipFlop::WatchValuesFlipFlop(
  WatchValuesBaseCore & associated_watch_)
  :
  associated_watch( associated_watch_),
  state(false),
  down_disable_models(),
  down_assignments(),
  down_actions()
{
  multi_shot = true;
  associated_watch.multi_shot = true;
  associated_watch.add_self_to_manager_active_list = false;
}
/****************************************************************************/
WatchValuesFlipFlopDelayed::WatchValuesFlipFlopDelayed(
  WatchValuesBaseCore & associated_watch_,
  const double & delay_variable_)
  :
  WatchValuesFlipFlop(associated_watch_),
  delay_variable( delay_variable_),
  in_delay(false),
  baseline_delay_value(0.0),
  delay_value(0.0),
  up_delay(0.0),
  down_delay(0.0)
{}

/*****************************************************************************
Destructor
*****************************************************************************/
WatchValuesFlipFlop::~WatchValuesFlipFlop()
{
  for (auto & assgt : down_assignments) {
    delete assgt;
  }
}

/*****************************************************************************
Name: initialize
Purpose:
  Override of the WatchValuesBaseCore::intialize() method to make sure the
  associated-watch also gets initialized.
*****************************************************************************/
void
WatchValuesFlipFlop::initialize(
       std::list<WatchValuesBaseCore *> * active_watches)
{
  associated_watch.initialize(active_watches);
  WatchValuesBaseCore::initialize(active_watches);
}

/*****************************************************************************
Name: test_crossing
Purpose:
  Override of the pure-virtual WatchValuesBaseCore::test_crossing.
Notes:
- In WatchValuesBase<T>::test_crosssing, the watch-variable is evaluated
  against the conditions, and the event_triggered and active flags set in
  response.
- In WatchValuesFlipFlop::test_crossing():
  - the conditions are tested in the call to associated_watch.test_crossing()
  - this event is triggered if the state of those conditions *changes*.
  - this event remains active until unsubscribed.
*****************************************************************************/
bool
WatchValuesFlipFlop::test_crossing()
{
  bool new_state = associated_watch.test_crossing();
  event_triggered = (new_state != state);
  state = new_state;
  return event_triggered;
}
//****************************************************************************
bool
WatchValuesFlipFlopDelayed::test_crossing()
{
  if (in_delay) {
    if ( evaluate_delay()) { // delay is complete
      in_delay = false;
      // Before transitioning, check that the transition is still needed
      return WatchValuesFlipFlop::test_crossing();
    }
    // else still in delay-hold, no transition.
    return false;
  }
  /* If not in a delay-pattern, test the crossing just as we would for any
   * other event, and if detected, start the delay process.*/
  bool new_state = associated_watch.test_crossing();
  // If no change, nothing to do; event is untriggered:
  if (new_state == state) {
    event_triggered = false;
  }

  /* underlying state has changed, start the delay (already know we are not
     in an active delay process).*/
  else {
    baseline_delay_value = delay_variable;
    delay_value = (new_state)? up_delay : down_delay;
    /* evaluate the delay; it could be configured to 0.0 in which case the
       delay will be ignored  and the event will trigger just as though it was
       a WatchValuesFlipFlop instance.*/
    if (evaluate_delay()) {
      event_triggered = true;
      state = new_state;
    }
    /* Otherwise, tag this instance as having a delay in-process.
       On the next calls to test_crossing(), execution will process through the
       first logic block until the delay condition has been satisfied.*/
    else {
      event_triggered = false;
      in_delay = true;
    }
  }
  /* If new_state has not changed from state, or if it has AND a delay-process
   * has started, then the event is not triggered.*/
  return event_triggered;
}


/*****************************************************************************
Name: apply_complementary_changes
Purpose:
  Override of the WatchValuesBaseCore::apply_complementary_changes() method
Notes:
- The inherited lists:
  - ext_bool_on
  - ext_bool_off
  - subscribe_models
  - unsubscribe_models
  - disable_models
  - assignments
  - actions
  are used as in WatchValuesBaseCore when this events state switches from
  false to true.
- Some of those same lists:
  - ext_bool_on
  - ext_bool_off
  - subscribe_models
  - unsubscribe_models
  are used in reverse when this event's state switches from true to false.
- Some of those lists are not reversible, but alternative lists:
  - down_disable
  - down_assignments
  - down_actions
  are provided; these are also used when this event's state switches from
  true to false
- Disabling a model is terminal. It cannot be re-enabled so the
  disable_models (and down_disable_models) lists are not reversible.
- This method is called from the Events-manager in response this event's
  event_triggered flag being set in test_crossing(). event_triggered is set
  to true for a false-to-true or true-to-false evaluation of the
  associated-watch. event_triggered is false if the triggered status of the
  associated-watch does not change.
*****************************************************************************/
void
WatchValuesFlipFlop::apply_complementary_changes()
{
  if (state) {
    WatchValuesBaseCore::apply_complementary_changes();
  } else {
    /* This may seem counterintuitive and erroneous, but this is intentionally
     * reversing the actions taken when the state flipped from false to
     * true.*/
    for (auto & x : ext_bool_on) {
      *x = false;
    }
    for (auto & x : ext_bool_off) {
      *x = true;
    }
    for (auto & x : subscribe_models) {
      x->unsubscribe();
    }
    for (auto & x : unsubscribe_models) {
      x->subscribe();
    }
    for (auto & x : down_disable_models) {
      x->disable();
    }
    for (auto & x : down_assignments) {
      x->make_assignment();
    }
    for (auto & x : down_actions) {
      x->specific_execution();
    }
    if (!message.empty()) {
      CMLMessage::inform( __FILE__,__LINE__,
        "Event processed:\n", message, "\nFlipped down.\n\n");
    }
  }
}

/*****************************************************************************
Name: activate
Purpose:
  Override of SubscriptionBase::activate
  Needed to make sure that the associated_watch is active
*****************************************************************************/
void
WatchValuesFlipFlop::activate()
{
  associated_watch.subscribe();
  // Initialize the state to whatever the associated_watch is generating right
  // now; we do not want a transition at activation.
  state = associated_watch.test_crossing();

  if (add_self_to_manager_active_list) {
    if (!active_watches) {
      CMLMessage::error( __FILE__,__LINE__,
        "Activating watch with internal instruction to add itself to\n"
        "the active-watch list, but has no access to that list.\n"
        "Watch will not get checked by the event-manager.\n");
    } else {
      active_watches->push_back(this);
    }
  }

  SubscriptionBase::activate();
}

/*****************************************************************************
Name: activate
Purpose:
  Override of SubscriptionBase::deactivate
  Removes the subscription from the associated_watch, likely deactivating it.
*****************************************************************************/
void
WatchValuesFlipFlop::deactivate()
{
  associated_watch.unsubscribe();
  SubscriptionBase::deactivate();
}

/*****************************************************************************
Name: evaluate_delay
Purpose:
  Returns true if the delay conditions have been satisfied.
*****************************************************************************/
bool
WatchValuesFlipFlopDelayed::evaluate_delay()
{
  return (std::abs( delay_variable - baseline_delay_value) >= delay_value);
}
