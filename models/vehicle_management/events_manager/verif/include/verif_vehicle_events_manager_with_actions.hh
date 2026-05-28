/*******************************TRICK HEADER******************************
PURPOSE: (Provide a template of a set of events and how they might be
      configured in a real simulation.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (2014) (Antares) (initial)))
**********************************************************************/
#ifndef VERIF_VEHICLE_EVENTS_MANAGER_HH
#define VERIF_VEHICLE_EVENTS_MANAGER_HH

#include "../../include/vehicle_events_manager.hh"
#include "../../include/watch_values_base.hh"
#include "../../include/watch_values_base_multi.hh"
#include "../../include/watch_values_delay.hh"
#include "../../include/watch_values_set.hh"
#include "../../include/watch_values_flipflop.hh"
#include "../../include/event_action.hh"
#include "cml/models/utilities/trick_logging/include/trick_logging.hh"

class VerifVehicleEventsManager : public VehicleEventsManager
{
 public:

  EventActionSimStopNow action_sim_stop_now;
  WatchValuesBase<double> sim_stop_var_verif; /* (--)
      stop the sim on variable */

  EventActionSimStopDyn action_sim_stop_dyn;
  WatchValuesBase<bool> sim_stop_verif; /* (--)
      stop the sim */

  WatchValuesBase<double> relative_to_activation_double_verif; /* (--)
      stop the sim on double variable relative to activation */

  EventActionDeactivateSimObject action_deact_sim_obj;
  WatchValuesBase<double> deact_sim_obj_verif; /* (--)
      turn off the sim object */

  EventActionAdjustLogging action_adjust_logging;
  WatchValuesBase<double> adjust_log_verif; /* (--)
      adjust the logging rates */

  EventActionAdjustLoggingGroup action_adjust_logging_group;
  WatchValuesBase<double> adjust_log_group1; /* (--) Adjust logging frequency*/
  WatchValuesBase<double> adjust_log_group2; /* (--) disable the group*/
  WatchValuesBase<double> adjust_log_group3; /* (--) enable to group */

  EventActionLogNow action_log_now;
  WatchValuesBase<double> log_now_verif; /* (--)
      Log content immediately regardless of schedule */

  WatchValuesBase2<double, bool> two_variable_check_and; /* (--)
      Test 2-conditionals event, where conditionals are combined with "and"*/
  WatchValuesBase2<double, bool> two_variable_check_or; /* (--)
      Test 2-conditionals event, where conditionals are combined with "or" */
  WatchValuesDelay<int> delayed_check; /* (--)
      Test a delayed response to a trigger */

  WatchValuesBase<bool>   trigger_set_bool;
  WatchValuesBase<int>    trigger_set_int;
  WatchValuesBase<double> trigger_set_double;
  WatchValuesSet  trigger_set_all; /* (--)
      A set of 3 triggers.*/
  WatchValuesSet  trigger_set_any; /* (--)
      A set of 3 triggers.*/
  WatchValuesSet  trigger_set_none; /* (--)
      A set of 0 triggers (error).*/

  WatchValuesBase<bool> base_bool_ff; /* (--)
      A version of base_bool_verif dedicated to testing flip_flop.*/
  WatchValuesFlipFlop flip_flop; /* (--)
    Utilizes base_bool_ff to flip-flop between outputs tates based on
    the input state.*/



 public:
  virtual void execution_follow_up(){};
  VerifVehicleEventsManager(TrickLogging & trick_logging,
                            const double  & time)
      :
      VehicleEventsManager(),
      action_adjust_logging(trick_logging),
      action_adjust_logging_group(trick_logging),
      action_log_now(trick_logging),
      delayed_check(time),
      flip_flop(base_bool_ff)
      {};

 private:
  VerifVehicleEventsManager (const VerifVehicleEventsManager& rhs);
  VerifVehicleEventsManager& operator = (const VerifVehicleEventsManager& rhs);

};
#endif
