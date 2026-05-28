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

NOTES:
- The flip-flop event is intended to be used when the conditions used to
  trigger-up are the logical inverse of those used to trigger-down.
  Those conditions may be complex, as long as there is a relation:
  - A implies up
  - not-A implies down
- A more involved logic, such as trigger-up if A but only return if
  not-A AND some other condition B is better suited to pairing two events,
  one to trigger-up and one to trigger-down. With such a pairing each event
  can be configured to activate the other, but each event can then be
  configured with its own unique set of conditions.

LIBRARY DEPENDENCY:
   ((../src/watch_values_flipflop.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (2026/02) (ANTARES) (initial implementation)))
*******************************************************************************/
#ifndef WATCH_VALUES_FLIPFLOP_HH
#define WATCH_VALUES_FLIPFLOP_HH

#include <list>

#include "watch_values_base_core.hh"

/*****************************************************************************
Name: WatchValuesFlipFlop
Purpose:
  A two-state event, an either-or relationship.
  If the conditions are satisfied, transition into one state. When they are
  no longer satisfied, transition back.
*****************************************************************************/
class WatchValuesFlipFlop : public WatchValuesBaseCore
{
 protected:
  WatchValuesBaseCore & associated_watch; /* (--)
    The associated-watch is kept as a reference rather than an instance
    to support any level of complexity or type in evaluating the
    simulation data for satisfying prescribed conditions.
    The associated_watch must be separately instantiated, and that isntance
    can be any derivative of WatchValuesBaseCore, including CompoundEvents.
    The associated_watch does not need to be added to the Events-Manager,
    although it can be and should be if it has specific-actions itself.
    */
  bool state; /* (--)
    Flag indicating the current triggered-status of the associated_watch
    Used to identify transitions in that status.*/

 public:
  std::list<SubscriptionBase *> down_disable_models; /* (--)
    list of models that will be disabled when the associated_watch
    event_triggered value switches from true to false. */

  std::list< EventVariableAssignmentBase * > down_assignments; /* (--)
    list of variable assignments to be applied when the associated_watch
    event_triggered value switches from true to false. */

  std::list< EventActionBase * > down_actions; /* (--)
    list of encapsulated actions to be executed when the associated_watch
    event_triggered value switches from true to false. */

  explicit WatchValuesFlipFlop( WatchValuesBaseCore & associated_watch);
  virtual ~WatchValuesFlipFlop();

  void initialize(std::list<WatchValuesBaseCore *> * active_watches) override;
  virtual bool test_crossing() override;
  void apply_complementary_changes() override;

  void add_to_down_disable(SubscriptionBase & add_me)
                                     {down_disable_models.push_back(&add_me);}
  template<typename T> void add_down_assignment( T& variable, T value)
  {
    down_assignments.push_back( new EventVariableAssignment<T>(variable,value));
  }
  template<typename T> void add_assignment_ref( T & variable, const T & value)
  {
    assignments.push_back( new EventVariableAssignmentRef<T>(variable,value));
  }
  void add_down_action( EventActionBase& action)
  {
    down_actions.push_back(&action);
  }
 protected:
  void activate() override;
  void deactivate() override;
 private:
  WatchValuesFlipFlop( const WatchValuesFlipFlop&) = delete;
  WatchValuesFlipFlop& operator=( const WatchValuesFlipFlop&) = delete;
};

/*****************************************************************************
Name: WatchValuesFlipFlopDelayed
Purpose:
  A WatchValuesFlipFlop where the transitions may be delayed, usually by time.
Note:
  If the delay is equally applied to up-transitions and down-transitions,
  using a WatchValuesDelay for the associated-watch makes more sense.
  The primary purpose of this class is to provide the two transitions
  with different delays, or to provide more flexibility to the configuration.
*****************************************************************************/
class WatchValuesFlipFlopDelayed : public WatchValuesFlipFlop
{
 protected:
  const double & delay_variable; /* (--)
    Reference to the variable used to delay the transition.
    This is typically to simulation dynamic-time.*/
  bool in_delay; /* (--)
    Flag indicating that the conditions have been satisfied to flip the state
    and we are just waiting for the delay to process.*/
  double baseline_delay_value; /* (--)
    The value of delay_variable when the delay-process starts.*/
  double delay_value; /* (--)
    A copy of either up_delay or down_delay, depending on which transition
    is pending. Considered only while in_delay = true.*/

 public:
  double up_delay; /* (--)
    The "delay" in the value of delay_variable when triggering up.
    This is the difference between the value of delay_variable at the time the
    transition conditions are satisfied and the value of delay_variable when
    the transition should be applied.*/
  double down_delay; /* (--)
    The "delay" in the value of delay_variable when triggering up.
    This is the difference between the value of delay_variable at the time the
    transition conditions are satisfied and the value of delay_variable when
    the transition should be applied.*/
  WatchValuesFlipFlopDelayed( WatchValuesBaseCore & associated_watch,
                              const double & delay_variable);
  bool test_crossing() override;
 protected:
  bool evaluate_delay();
 private:
  WatchValuesFlipFlopDelayed( const WatchValuesFlipFlopDelayed&) = delete;
  WatchValuesFlipFlopDelayed& operator=(
                              const WatchValuesFlipFlopDelayed&) = delete;
};
#endif
