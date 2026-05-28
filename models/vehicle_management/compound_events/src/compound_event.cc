/********************************* TRICK HEADER *******************************

PURPOSE:
   (Define checking process for each event managed by event manager)

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
#include "../include/compound_event.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
CompoundEvent::CompoundEvent(
  const double & time_)
  :
  WatchValuesDelay(time_),
  time(time_),
  name(),
  arming_triggers(),
  disarming_triggers(),
  action_triggers(),
  allow_immediate_actions(false),
  deactivate_at_disarm(false),
  arm_trigger_check_period(false),
  disarm_trigger_check_period(0.0),
  action_trigger_check_period(0.0),
  event_has_action_triggers(false),
  time_triggered(0.0),
  next_arming_check(0.0),
  next_disarming_check(0.0),
  next_action_check(0.0),
  discrete_time_elapsed(0.0),
  number_of_times_armed(0),
  status(Unarmed)
{
  subscribe_name = "CompoundEvent: " + name;
  arming_triggers.subscribe_name = subscribe_name + " : arming_triggers";
  disarming_triggers.subscribe_name = subscribe_name + " : disarming_triggers";
  action_triggers.subscribe_name = subscribe_name + " : action_triggers";

  arming_triggers.add_self_to_manager_active_list = false;
  disarming_triggers.add_self_to_manager_active_list = false;
  action_triggers.add_self_to_manager_active_list = false;

  // default -- see documentation for (non-trivial) explanation:
  action_triggers.multi_shot = true;

  set_watch (action_triggers.event_triggered, true);
}

/*****************************************************************************
initialize
Purpose:
  Initialize the event and its triggers as configured at sim-initialization.
*****************************************************************************/
void
CompoundEvent::initialize(std::list<WatchValuesBaseCore *> * active_watches)
{
  if (!enabled) {return;}

  if ((arming_triggers.get_num_triggers() == 0) &&
      (action_triggers.get_num_triggers() == 0)) {
    CMLMessage::error( __FILE__,__LINE__,
      "No triggers were assigned to compound-event ",name,
      ".\n Event will not be initialized.\n");
    return;
  }

  /* Note: - Putting the event initialization before the trigger
   * initializations to avoid inadvertent triggering at initialization.
   * If the whole thing is already subscribed (e.g. from the input file),
   * initialization will lead to activation and a subsequent update() call.
   * But if the triggers are not initialized, the update will result in no
   * action. If the triggers were to be initialized first, then the event
   * initialization could result in a detection based on poory-configured or
   * not-configured data values that the triggers are monitoring.*/
  WatchValuesDelay::initialize( active_watches);

  // Note -- we do not need to add the WatchValuesSet to the Events
  // Manager because this event is managing them separately.
  arming_triggers.add_self_to_manager_active_list = false;
  disarming_triggers.add_self_to_manager_active_list = false;
  action_triggers.add_self_to_manager_active_list = false;

  if (arming_triggers.get_num_triggers() > 0) {
    arming_triggers.initialize( active_watches);
    if (disarming_triggers.get_num_triggers() > 0) {
      disarming_triggers.initialize( active_watches);
    }
  }

  if (action_triggers.get_num_triggers() > 0) {
    action_triggers.initialize( active_watches);
    event_has_action_triggers = true;
  }
}


/*****************************************************************************
test_crossing
Purpose:
  Extends WatchValuesDelay::test_crossing() by adding in the arming/disarming
  checks.
*****************************************************************************/
bool
CompoundEvent::test_crossing()
{
  if (!active) {return false;}

  /* If the event has active arming trigger(s), and the arming trigger-set is
   * scheduled for testing, test it and set the time for the next test.
   * Notes:
   * - if there is no arming trigger, status modes immediately to
   *   Armed at initialization and arming_triggers will not be active.
   * - Once triggered, the arming-trigger will either deactivate itself
   *   (remaining triggered), or if it is configured as multi-shot, it will
   *   remain active and the conditions will be tested over and over again.
   * - The arming trigger may therefore need to be rechecked regardless of
   *   current status, so check it before we get into the switch statement.*/
  if ( arming_triggers.is_active() &&
       (time >= next_arming_check)) {
    next_arming_check = time + arm_trigger_check_period;
    arming_triggers.test_crossing();
  }
  test_crossing_iterative();
  return event_triggered;
}

/*****************************************************************************
test_crossing
Purpose:
  FIXME Comment Me
*****************************************************************************/
void 
CompoundEvent::test_crossing_iterative()
{
  switch (status) {
  case Unarmed:
    if (arming_triggers.event_triggered) {
      /* A new positive result. Change status, apply associated complementary
       * changes and subscribe to the follow-on triggers.*/
      if (event_has_action_triggers) {
        status = Armed;
        action_triggers.subscribe();
      }
      else {
        status = ArmedNoAction;
        event_triggered = true;
        /* If there are no action trigggers, but the event is armed,
         * we want to set event_triggered to true, it is set to false
         * down below*/
      }
      /* Note -- this is not obvious, but it is necessary to manage the
       * activity of the triggers accessed by disarming_triggers. If those
       * triggers are inactive or use activation-relative behavior, they need
       * to be reactivated and that can be done by resubscribing -- and
       * therefore reactivating -- disarming_triggers.
       * If there are no disarm-triggers, subscribing does nothing.*/
      disarming_triggers.subscribe();
      arming_triggers.apply_complementary_changes();
      number_of_times_armed++;
      CMLMessage::debug(__FILE__,__LINE__,
        "CompoundEvent \"",name,"\" armed.\n");
      /* If we are allowing immediate follow-on actions -- such as
       * immediately disarming, or immediately evaluating the action-triggers
       * -- then re-run test_crossing with the new status.
       * Note -- An ArmedNoAction status will indicate event.event_triggered = true,
       * which will be set to false if the disarming triggers are triggered.
       * If immediate actions are set to false, an event can only be untriggered
       * on the next cycle.*/
      if (allow_immediate_actions) {
        test_crossing_iterative();
      }
    }
    break;


  case Armed:
    /* If the arming trigger is being regularly checked and is no longer
     * triggered, drop back to Unarmed status and remove the action_triggers
     * and disarming_triggers subscriptions. See documentation for why this is
     * important.*/
    if (arming_triggers.is_active() && !arming_triggers.event_triggered) {
      action_triggers.unsubscribe();
      disarming_triggers.unsubscribe();
      status = Unarmed;
      CMLMessage::debug(__FILE__,__LINE__,
        "CompoundEvent \"",name,"\" unarmed.\n");
      break;
    }
    /* Otherwise, check for forced-disarming:
     * If the disarming triggers are active and scheduled, check them.*/
    if( disarming_triggers.is_active() &&
        (time >= next_disarming_check)) {
      next_disarming_check = time + disarm_trigger_check_period;
      if (disarming_triggers.test_crossing()) {
        /* If disarming satisified:
        * (And deactivate_at_disarm):
        * - unsubscribe all triggers and deactivate the event.
        * (Else):
        * - make sure the arming triggers are (re-)activated by issuing a
        *   renewed subscription.
        * - unsubscribe from the action-triggers.*/
        if (deactivate_at_disarm) {
          arming_triggers.unsubscribe();
          action_triggers.unsubscribe();
          disarming_triggers.unsubscribe();
          deactivate();
        }
        else {
          arming_triggers.subscribe();
          action_triggers.unsubscribe();
          disarming_triggers.unsubscribe();
          status = Unarmed;
        }
        event_triggered = false;
        CMLMessage::debug(__FILE__,__LINE__,
          "CompoundEvent \"",name,"\" disarmed.\n");
        break;
      }
      // else, keep going to check the action-triggers
    }

    // Finally, if still armed, check the action triggers,
    // If there are no action-triggers, the action-trigger is set to triggered
    // automatically with Armed status.*/
    // If there are action-triggers, check them:*/
    if (action_triggers.is_active() &&
        (time >= next_action_check)) {
      next_action_check = time + action_trigger_check_period;
      // The WatchValuesDelay::test_crossing() will only get executed if the
      // trigger-action set is triggered. This would normally reset the
      // event-triggered flag to false for a multi-shot event that is no longer
      // triggered ... but that won't happen if the action_triggers set does not
      // trigger. So reset event_triggered now regardless, and it will flip
      // up again if the event really is freshly triggered.
      event_triggered = false;
      if (action_triggers.test_crossing()) {
        // If the action-triggers are satisfied, we apply any complementaary
        // changes associated with satisfcction of the action-triggers,
        // and in case where the triggers are monitoring extremum values
        // (e.g. max/min), update the reference value to the newly detected
        // value, forcing the trigger to exceed that value to trigger again.*/
        action_triggers.apply_complementary_changes();
        action_triggers.update_conditional_trigger_references();
        // record the time at which the event was triggered, without including
        // the delay, and post the debug message.:
        time_triggered = time;
        CMLMessage::debug(__FILE__,__LINE__,
          "CompoundEvent \"",name,"\" action-trigger satisfied.\n");

        // Now mode to the next status.
        // Set the WatchValuesDelay member flag to indicate that we are
        // starting a new delay cycle.
        event_triggered_pending_delay = false;
        // Query whether we can go straight to completion with no delay.
        // Note -- WatchValuesDelay::test_crossing() queries
        //         action_triggers.event_triggered, with consideration of the
        //         compound-events's specified delay. If it returns true,
        //         there is no delay configured.
        if (WatchValuesDelay::test_crossing()) {
          status = (multi_shot)? Armed : PostAction;
          discrete_time_elapsed = 0.0;
        }
        else {
          status = HoldAction;
          action_triggers.unsubscribe();
          disarming_triggers.unsubscribe();
        }
      }
    }
    break;

  case ArmedNoAction:
    /* Perform very similar disarming checks to those performed in the Armed
     * status. Comments not repeated, see Armed case for details.*/
    if (!arming_triggers.event_triggered) {
      disarming_triggers.unsubscribe();
      status = Unarmed;
      CMLMessage::debug(__FILE__,__LINE__,
        "CompoundEvent \"",name,"\" unarmed.\n");
      break;
    }
    if( disarming_triggers.is_active() &&
       (time >= next_disarming_check)) {
      next_disarming_check = time + disarm_trigger_check_period;
      if (disarming_triggers.test_crossing()) {
        CMLMessage::debug(__FILE__,__LINE__,
          "CompoundEvent \"",name,"\" disarmed.\n");
        event_triggered = false;
        /* If event is disarmed set event_triggered to be false, since it
         * has been set to true when status was set to ArmedNoAction */
        /* If event is configured with deactivate_at_disarm, deactivate the event,
         * Otherwise, check multi_shot flag to return to Unarmed status
         * If neither flag is set, set status to PostAction. */
        if (deactivate_at_disarm) {
          if (multi_shot) {
            CMLMessage::warn(__FILE__,__LINE__,
              "CompoundEvent \"",name,"\" deactivate_at_disarm and multi_shot\n"
              "have been set true, deactivate_at_disarm trumps multi_shot, so\n"
              "the event has been deactivated.\n");
          }
          deactivate();
        }
        else if (multi_shot) {
          if (!arming_triggers.is_active()) {
            arming_triggers.subscribe();
          }
          disarming_triggers.unsubscribe();
          status = Unarmed;
        } else {
          status = PostAction;
        }
      }
    }
    break;

  case HoldAction:
    /* Note --
     * This next step is the main event, monitoring for
     * action_triggers.event_triggered going true. After the delay is applied
     * (which will be immediately if the delay is at the default 0.0), this
     * event's event-triggered flag will go true and the events-manager will
     * be able to process all of the capabilities of the event in response
     * to its completion.*/
    if (WatchValuesDelay::test_crossing()) {
      if (multi_shot) {
        status = Armed;
        action_triggers.subscribe();
        disarming_triggers.subscribe();
      }
      else {
        status = PostAction;
      }
      discrete_time_elapsed = time - time_triggered;
    }
    break;
  case PostAction:
    deactivate();
    break;
  }
}

/*****************************************************************************
update_no_manager
Purpose:
  Provides an update interface to execute an event in the absence of
  an event-manager. Note that an event being accessed multiple times from
  different places may result in multiple calls to
  apply_complementary_changes().
Notes:
  returns true if event has satsified all requried conditions, which could
  be achieved by either:
    - event has been triggered by satisfying action-triggers or
    - event has reached terminal ArmedNoAction status (has no action triggers).
*****************************************************************************/
bool
CompoundEvent::update_no_manager()
{
  if (test_crossing()) {
    apply_complementary_changes();
  }
  return event_triggered;
}

/*****************************************************************************
activate
Purpose: starts the event.
*****************************************************************************/
void
CompoundEvent::activate()
{
  if (arming_triggers.get_num_triggers() > 0) {
    status = Unarmed;
    arming_triggers.subscribe();
  }
  else {
    status = Armed;
    action_triggers.subscribe();
  }
  WatchValuesDelay::activate();

  // Identify which of the triggers has a conditional reference value.
  action_triggers.populate_conditional_reference_trigger_list();
}
