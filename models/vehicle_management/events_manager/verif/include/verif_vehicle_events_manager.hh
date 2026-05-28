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
#include "../../include/watch_values_specific.hh"
#include "../../include/watch_values_flipflop.hh"
#include "cml/models/utilities/trick_logging/include/trick_logging.hh"

class VerifVehicleEventsManager : public VehicleEventsManager
{
 public:

  WatchValuesSimStopVar<double> sim_stop_var_verif; /* (--)
      stop the sim on variable */
  WatchValuesSimStop sim_stop_verif; /* (--)
      stop the sim */
  WatchValuesSimStopVar<double> relative_to_activation_double_verif; /* (--)
      stop the sim on double variable relative to activation */
  WatchValuesDeactivateSimObject deact_sim_obj_verif; /* (--)
      turn off the sim object */
  WatchValuesAdjustLogging<double> adjust_log_verif; /* (--)
      adjust the logging rates */
  WatchValuesLogNow<double> log_now_verif; /* (--)
      Log content immediately regardless of schedule */
  WatchValuesBase<double> base_double_verif; /* (--)
      Test generic watch based on a "double" variable*/
  WatchValuesBase<bool> base_bool_verif; /* (--)
      Test generic watch based on a "bool" variable*/
  WatchValuesBase<double> reference_var_verif; /* (--)
      Test generic watch based on a reference variable*/
  WatchValuesBase<double> bad_variable_verif; /* (--)
      Test generic watch that is meant to fail due to bad variable*/
  WatchValuesBase<int> relative_to_activation_int_verif; /* (--)
      Test generic watch based on an "int" variable relative to activation */
  WatchValuesBase<bool> relative_to_activation_bool_verif; /* (--)
      Test generic watch based on a "bool" variable relative to activation */
  WatchValuesBase<float> relative_to_activation_float_verif; /* (--)
      Test generic watch based on a "float" variable relative to activation */
  WatchValuesBase2<double, bool> two_variable_check_and; /* (--)
      Test 2-conditionals event, where conditionals are combined with "and"*/
  WatchValuesBase2<double, bool> two_variable_check_or; /* (--)
      Test 2-conditionals event, where conditionals are combined with "or" */
  WatchValuesDelay<int> delayed_check; /* (--)
      Test a delayed response to a trigger */

  WatchValuesBase<unsigned int> relative_to_activation_uint_verif; /* (--)
      Error test case: set_relative_to_activation not defined for uint */

  WatchValuesBase<bool> base_bool_ff; /* (--)
      A version of base_bool_verif dedicated to testing flip_flop.*/
  WatchValuesFlipFlop flip_flop; /* (--)
    Utilizes base_bool_ff to flip-flop between outputs tates based on
    the input state.*/

  WatchValuesBase<float> unconfigured_event; /* (--)
      Error test case: An unconfigured event */

 public:
  virtual void execution_follow_up(){};
  VerifVehicleEventsManager(TrickLogging & trick_logging,
                            const double  & time)
      :
      VehicleEventsManager(),
      adjust_log_verif(trick_logging),
      log_now_verif(trick_logging),
      delayed_check(time),
      flip_flop(base_bool_ff)
      {};

 private:
  VerifVehicleEventsManager (const VerifVehicleEventsManager& rhs);
  VerifVehicleEventsManager& operator = (const VerifVehicleEventsManager& rhs);

};
#endif
