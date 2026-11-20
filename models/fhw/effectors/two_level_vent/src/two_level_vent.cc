/*******************************TRICK HEADER******************************
PURPOSE: (Wrapper Source for defining and applying vent disturbances to candidate vehicle)

PROGRAMMERS:
  (((Suyog Benegalrao & Alexandre Masset) (JSC) (Jul 2025) (Antares) (initial version)))
 ************************************************************************/

#include "../include/two_level_vent.hh"
#include "../include/two_level_vent_set.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

/***************************************************************************************************
Constructors
***************************************************************************************************/
TwoLevelVent::TwoLevelVent(
     const double& dyn_time_,
     TwoLevelVentSet& vent_set)
  :
  SimpleVent( dyn_time_),

  wait_time_min(0.0),
  wait_time_max(0.0),
  high_duration_min(0.0),
  high_duration_max(0.0),
  wait_time_randFlag(true),
  high_duration_randFlag(true),

  accel_mag_high(0.0),
  accel_mag_low(0.0),
  high_duration(0.0),
  high_wait_time(0.0),
  dv_max(0.0),
  action_reset(this),

  vehicle_mass(vent_set.vehicle_mass),
  gen(vent_set.gen),
  dv_mag(0.0),
  sched_onTime(0.0),
  high_venting(false),
  start_time_high(0.0)
{
    // The vent adds itself to the referenced vent set
    // Note that this means the vent cannot be part of multiple vent sets.
    quiet_message_start_inactive = true;
    vent_set.add_vent(*this);
}

/***************************************************************************************************
TwoLevelVent
check_configuration: Override method to check vents configuration before running the sim
NOTE:
    - The model cannot run correctly if high mode firings duration is longer than the expected time
      between two firings. This could result in only high mode firing (no randomization) or unexpected 
      longer high mode firing.
    - Therefore, the maximum high mode duration has to be larger than the minimum
      wait time between two high mode firings. 
    - This override method checks if it needs to turn off parameter randomization.
    - These design choices are motivated by the fact that a single high mode could be modeled with
      a simple vent instead.
***************************************************************************************************/
bool TwoLevelVent::check_configuration()
{
    if (high_duration_randFlag) {
        /* With this flag, we expect high_duration_max and high_duration_min to be set. 
        high_duration is updated when switching mode. We update it here to check config
        since high_duration_max is the critical parameter here. See 3rd check.*/
        high_duration = high_duration_max;    
    } else {
        /* Not using max and min duration. Setting them to high duration. */
        high_duration_max =
        high_duration_min = high_duration;
    }
    if (wait_time_randFlag) {
        /* With this flag, we expect wait_time_min and wait_time_max to be set.  
        high_wait_time is updated when switching mode. We update it here to check config
        since wait_time_min is the critical parameter here. See 3rd check.*/
        high_wait_time = wait_time_min;
    } else{
        /* Not using max and min duration. Setting them to high wait time. */
        wait_time_max =
        wait_time_min = high_wait_time;
    }

    if (high_duration_randFlag && (high_duration_min > high_duration_max)){
        CMLMessage::warn(
        __FILE__,__LINE__,"Illegal setup\n",
        "High mode minimum duration is set higher than maximum duration.\n",
        "Turning duration randomization off. Setting high mode duration to high_duration_max value.\n"
        "Check vent ", name, " configuration.\n");
        high_duration_min =
        high_duration = high_duration_max;
        high_duration_randFlag = false;
    }
    if (wait_time_randFlag && (wait_time_min >  wait_time_max)){
        CMLMessage::warn(
        __FILE__,__LINE__,"Illegal setup\n",
        "High mode minimum wait time is set higher than maximum wait time.\n",
        "Turning wait time randomization off. Setting high mode wait time to wait_time_min value.\n"
        "Check vent ", name, " configuration.\n");
        wait_time_max =
        high_wait_time = wait_time_min;
        wait_time_randFlag = false;
    }
    if (high_duration >= high_wait_time) {
        CMLMessage::error(
        __FILE__,__LINE__,"Illegal setup\n",
        "High mode duration is set higher than high mode wait time wait time.\n"
        "This can cause longer high mode firings that are not desired.\n"
        "Check vent ", name, " configuration.\n");
    }
    /* Initialize two_level vent to low venting mode */
    low_venting_mode();
    return SimpleVent::check_configuration();
}


/***************************************************************************************************
TwoLevelVent
reset: Resets the scheduled time variable and default to low venting
***************************************************************************************************/
void
TwoLevelVent::reset()
{
    sched_onTime = 0.0;
    low_venting_mode();
}

/***************************************************************************************************
TwoLevelVent
stop_venting: Override method to set the vent back to its low mode by default
***************************************************************************************************/
void TwoLevelVent::stop_venting()
{
  // Default to low mode is necessary to guarantee a high mode when the vent starts again.
  reset();
  // Set venting flag to false
  SimpleVent::stop_venting();
}

/***************************************************************************************************
TwoLevelVent
update_force: Override method called in SimpleVent::update.
Sets low or high venting mode depending on current schedule
***************************************************************************************************/
void TwoLevelVent::update_force()
{
    /* Skip vent if not active */
    if (!active || !venting) {return;}
    /* conditions to meet in order to turn on vent */
    if (high_venting) {
        // If the high scheduled venting event has reached its duration, switch to low mode.
        if (dyn_time - start_time_high >= high_duration) {
            low_venting_mode();
        }
    } else {
        // If the scheduled venting event has been reached, switch to high mode.
        if (dyn_time >= sched_onTime && high_duration > 0.0 &&
            accel_mag_high > 0.0) { high_venting_mode();}
    }
}


/***************************************************************************************************
TwoLevelVent
low_venting_mode: Set low venting mode
***************************************************************************************************/
void TwoLevelVent::low_venting_mode()
{
    /* low venting */    
    high_venting = false;

    /* reset force-related flags */
    unset_force_magnitude();

    /* Set Impulse to Low Magnitude */
    set_force_magnitude(vehicle_mass * accel_mag_low, false);
}


/***************************************************************************************************
TwoLevelVent
high_venting_mode: Set high venting mode
***************************************************************************************************/
void TwoLevelVent::high_venting_mode()
{
    /* high venting */
    high_venting = true;
    /* reset force-related flags */
    unset_force_magnitude();

    /* set vent disturbance duration */
    if (high_duration_randFlag){
        high_duration = std::uniform_real_distribution<>{high_duration_min, high_duration_max}(gen);
    }

    /* Check if current delta-V is larger than max delta-V
    If yes, rescale duration to match maximum delta-V
    */
    if (accel_mag_high * high_duration > dv_max){
        CMLMessage::warn(
            __FILE__,__LINE__,
            "High mode venting duration and acceleration results\n",
            "in Delta-V larger than accepted maximum.\n",
            "Resetting duration to match maximum delta-V for vent ", name, ".\n");
        high_duration = dv_max / accel_mag_high;
    }
    dv_mag = accel_mag_high * high_duration;

    /* schedule next vent start */
    if (wait_time_randFlag){
        high_wait_time = std::uniform_real_distribution<>{wait_time_min, wait_time_max}(gen);
    }
    sched_onTime = dyn_time + high_wait_time;

    start_time_high = dyn_time;

    /* Set force magnitude */
    set_force_magnitude(vehicle_mass * accel_mag_high, false);
}

/***************************************************************************************************
TwoLevelVentReset
specific_execution: Call TwoLevelVent::reset through an EventAction instance
***************************************************************************************************/
bool TwoLevelVentReset::specific_execution() {
    two_level_vent->reset();
    return false;
}
