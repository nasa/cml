/*******************************TRICK HEADER******************************
PURPOSE: (Simple vent force model - calculates force and torque from a vent
          but does not model mass depletion)

PROGRAMMERS:
  (((Daniel Ghan) (OSR) (Mar 2020) (Antares) (Initial version)))
 ************************************************************************/

#include "../include/vent.hh"

// JEOD header
#include "utils/math/include/vector3.hh"

/*************************************************************************
Constructor
*************************************************************************/
SimpleVent::SimpleVent( const double& dyn_time)
  :
  dyn_time(dyn_time),
  location{0.0, 0.0, 0.0},
  active(true),
  indefinite_duration(false),
  quiet_message_start_inactive(false),
  initialized(false),
  apply_as_impulse(false),
  venting(false),
  indefinite_duration_warning_sent(false),
  allocated_in_set(false),
  force_mag(0.0),
  impulse_mag(0.0),
  duration(0.0),
  start_time(0.0),
  stop_time(0.0),
  direction{0.0, 0.0, 0.0},
  force{0.0, 0.0, 0.0},
  impulse{0.0, 0.0, 0.0},
  direction_set(false),
  force_set(false),
  force_mag_set(false),
  impulse_set(false),
  impulse_mag_set(false),
  duration_set(false),
  user_set_impulse(0.0),
  user_set_duration(0.0),
  name("no-name")
{}

/*****************************************************************************
use_impulse_mode
Purpose:(Switches between impulse and dynamic (force) modes)
*****************************************************************************/
void SimpleVent::use_impulse_mode(bool mode)
{
  // If already in this mode, do nothing.
  if (mode == apply_as_impulse) {
    return;
  }

  // If turning the mode on, need to check for the availability of the
  // specified impulse.  If the model has not yet been initialized, then
  // this check will be performed at initialization.
  if (mode) {
    // pre-initialization; checks can still be made at initialization so
    // just change the flag without further consideration:
    if (!initialized) {
      apply_as_impulse = true;
    }
    // else: post-initialization; need to make appropriate checks that would
    // have been carried out at initialization here instead.

    // Test 1: Make sure the vent has an impulse available to use.
    else if (!impulse_set) {
      CMLMessage::error(__FILE__, __LINE__, "Unable to change modes\n",
        "Vent '", name, "': Cannot transition to impulsive mode because\n"
        "the impulse vector has not been set.\n");
    }
    // Test 2: Check to see if the impulse value has changed since
    // initialization; this could be the result of an external post-init
    // setting.
    else if ( MathUtils::has_changed_from( impulse_mag,
                                           user_set_impulse)) {
      apply_as_impulse = true;
      CMLMessage::warn(__FILE__, __LINE__, "Impulse magnitude changed\n",
        "The impulse magnitude was overwritten while vent '", name, "' was in\n"
        "dynamic mode.\nUser-set value: ", user_set_impulse, " N*s\nCurrent value: ", impulse_mag, " N*s\n");
    }
    // else: post-init, impulse exists and matches that from initialization.
    else {
      apply_as_impulse = true;
    }
  }

  // If turning the mode off, need to check for the availabilility of the
  // specified force using similar logic to that for the impulse immediately
  // above.
  else {
    if (!initialized) {
      apply_as_impulse = false;
    }
    else if (!force_set) {
      CMLMessage::error(__FILE__, __LINE__, "Unable to change modes\n",
        "Vent '", name, "': Cannot transition to dynamic (force) mode because\n"
        "the force vector has not been set.\n");
    }
    else if (MathUtils::has_changed_from( duration,
                                          user_set_duration)) {
      apply_as_impulse = false;
      CMLMessage::warn(__FILE__, __LINE__, "Duration changed\n",
        "The duration was overwritten while vent '", name, "' was in impulse mode.\n"
        "User-set value: ", user_set_duration, " s\nCurrent value: ", duration, " s\n");
    }
    else {
      apply_as_impulse = false;
    }
  }
}

/*************************************************************************
set_direction
Purpose:(Normalizes and sets the direction in which forces and impulses
         will be applied.)
*************************************************************************/
void SimpleVent::set_direction(double direction_in[3])
{
  // Normalize the vector
  jeod::Vector3::normalize(direction_in, direction);
  direction_set = true;
  // If initialized, reorient the force and impulse vectors
  if (initialized) {
    if (force_mag_set) {
      jeod::Vector3::scale( direction,
                            force_mag,
                            force);
      force_set = true;
    }
    if (impulse_mag_set) {
      jeod::Vector3::scale( direction,
                            impulse_mag,
                            impulse);
      impulse_set = true;
    }
  }
}


/*****************************************************************************
set_duration
Purpose:(Sets or unsets the duration)
*****************************************************************************/
void SimpleVent::set_duration( double duration_, bool hold_force)
{
  // Use an invalid setting as a proxy for unsetting the duration. If the vent
  // is currently venting, it will stop as soon as update() is called.
  if (duration_ <= 0.0) {
    duration = 0.0;
    duration_set = false;
  }
  else {
    user_set_duration =
    duration          = duration_;
    duration_set = true;
    indefinite_duration = false;
  }

  // If this vent is currently venting, adjust its stop time
  if (venting) {
    stop_time = start_time + duration;
  }

  // Assume here that if the duration of the burn is changed mid-flight, the
  // purpose is to adjust the total impulse rather than the instantaneous force.
  if (initialized) {
    // If hold_force == true (default), the model will modify the impulse
    // to accommodate the new duration.
    if (hold_force) {
      if (force_mag_set && duration_set) {
        set_impulse_internal();
      }
      // else no action.
    } else {
      // hold_force = false; the model will retain the existing
      // impulse and modify the force to accommodate the new duration.
      // If the impulse has not been set, this will have no effect.
      set_force_internal();
    }
  }
}

/*************************************************************************
set_force_magnitude
Purpose:(Sets the force magnitude)
*************************************************************************/
void SimpleVent::set_force_magnitude(double force_, bool hold_impulse)
{
  force_mag = force_;
  force_mag_set = true;
  if (initialized) {
    // typically, direction will be sett if the model is initialized, but
    // there is the manual unset_direction() that could unset it.
    if (direction_set) {
      jeod::Vector3::scale( direction,
                            force_mag,
                            force);
      force_set = true;
    }

    if (!hold_impulse) { // Default behaviour
    // To support the use-case where the vent is operating in impulse mode but
    // specified by a force/duration combination, modify the impulse in line
    // with the modified force.
      set_impulse_internal();
    }
    else {
      set_duration_internal();
    }
  }
}

/*************************************************************************
set_force_vector
Purpose:(Sets the force magnitude and vent direction from a force vector.)
*************************************************************************/
void SimpleVent::set_force_vector(double force_[3], bool hold_impulse)
{
  jeod::Vector3::copy( force_, force);
  force_set = true;

  if (initialized) {
    force_mag = jeod::Vector3::vmag(force);
    force_mag_set = true;

    if (force_mag <= 0.0) {
      CMLMessage::warn(__FILE__, __LINE__, "No direction change\n",
        "Vent '", name, "': Zero vector passed to set_force_vector.\n"
        "Force magnitude has been set to 0 but direction will not change.\n");
    } else {
      jeod::Vector3::scale( force_,
                            (1/force_mag),
                            direction);

      if (!hold_impulse) { // Default behaviour
        set_impulse_internal();
      }
      else {
        set_duration_internal();
      }
    }
  }
}

/*************************************************************************
set_impulse_magnitude
Purpose:(Sets the impulse magnitude)
*************************************************************************/
void SimpleVent::set_impulse_magnitude(double impulse_, bool hold_force)
{
  user_set_impulse = impulse_mag = impulse_;
  impulse_mag_set = true;
  if (initialized) {
    // typically, direction will be sett if the model is initialized, but
    // there is the manual unset_direction() that could unset it.
    if (direction_set) {
      jeod::Vector3::scale( direction,
                            impulse_mag,
                            impulse);
      impulse_set = true;
    }

    // If hold_force == true (default), the vent will modify the duration of
    // the vent to accommodate the new impulse.
    // If hold_force == false, the vent will hold the duration unchanged and
    // modify the force magnitude instead.

    if (hold_force) { //default behavior
      if (force_mag_set) {
        set_duration_internal();
      }
      // else -- holding to an unspecified force, no action to be taken.
      //   With an unspecified force, we could be operating in impulse mode
      //   with force and duration not relevant, or be in the middle of a
      //   reconfiguration
    }
    else if (duration_set) {
      // hold_force = false so force can be modified.
      // Try set_force_internal(), using the set duration to translate the
      // new impulse into a neww force.
      set_force_internal();
    }
    else if (!apply_as_impulse) {
      // not holding force, and no duration set.
      // If operating in dynamic mode, we need to translate the new
      // impulse value into a new force, but without a duration,
      // that is not possible.
      CMLMessage::warn(
        __FILE__,__LINE__,"Invalid specification\n",
        "Setting the impulse while operating in dynamic-mode with no\n"
        "specified duration cannot be translated into a force effect.\n"
        "Setting not applied to force values.\n");
    }
    // else operating in impulse mode, so force and duration are
    // irrelevant.
  }
}

/*************************************************************************
set_impulse_vector
Purpose:(Sets the impulse magnitude and vent direction from an impulse vector.
         impulse is primarily used when the vent is configured to
         apply an impulsive/instantaneous event but can also be used to
         specify a total impulse to be applied over some specified time
         interval.)
*************************************************************************/
void SimpleVent::set_impulse_vector(double impulse_[3], bool hold_force_mag)
{
  jeod::Vector3::copy( impulse_, impulse);
  impulse_set = true;

  if (initialized) {
    user_set_impulse = impulse_mag = jeod::Vector3::vmag(impulse);
    impulse_mag_set = true;

    if (impulse_mag <= 0.0) {
      CMLMessage::warn(__FILE__, __LINE__, "No direction change\n",
        "Vent '", name, "': Zero vector passed to set_impulse_vector.\n"
        "Impulse magnitude has been set to 0 but direction will not change.\n");
    } else {
      jeod::Vector3::scale( impulse_,
                            MathUtils::divide_protected( 1, impulse_mag, 0, false),
                            direction);
    }
    // Follow the same logic as set_impulse_magnitude, with the
    // additional step of setting the direction.
    if (hold_force_mag) {
      if (force_mag_set) {
        set_duration_internal();
        jeod::Vector3::scale( direction,
                              force_mag,
                              force);
      }
    }
    else if (duration_set) {
      set_force_internal();
    }
    else if (!apply_as_impulse) {
      CMLMessage::warn(
        __FILE__,__LINE__,"Invalid specification\n",
        "Setting the impulse while operating in dynamic-mode with no\n"
        "specified duration cannot be translated into a force effect.\n"
        "Setting not applied to force values.\n");
    }
  }
}

/*****************************************************************************
set_flowrate
Purpose:(Placeholder for Vent class)
*****************************************************************************/
void SimpleVent::set_flowrate( double val, bool flag)
{
  (void) val;
  (void) flag;
  CMLMessage::error(
    __FILE__,__LINE__,"Invalid setting\n",
    "A simple vent '", name, "' does not model mass flow; it has no flowrate");
}

/*****************************************************************************
set_exhaust_speed
Purpose:(Placeholder for Vent class)
*****************************************************************************/
void SimpleVent::set_exhaust_speed( double val, bool flag)
{
  (void) val;
  (void) flag;
  CMLMessage::error(
    __FILE__,__LINE__,"Invalid setting\n",
    "A simple vent '", name, "' does not model mass flow; it has no exhaust speed");
}

/*****************************************************************************
get_flowrate
Purpose:(Placeholder for Vent class)
*****************************************************************************/
double SimpleVent::get_flowrate()
{
  CMLMessage::error(
    __FILE__,__LINE__,"Invalid request\n",
    "A simple vent '", name, "' does not model mass flow; it has no flowrate");
  return 0.0;
}

/*****************************************************************************
get_exhaust_speed
Purpose:(Placeholder for Vent class)
*****************************************************************************/
double SimpleVent::get_exhaust_speed()
{
  CMLMessage::error(
    __FILE__,__LINE__,"Invalid request\n",
    "A simple vent '", name, "' does not model massflow; it has no exhaust speed.");
  return 0.0;
}

/*****************************************************************************
initialize
Purpose:(Checks all parameters have been set and marks vent as ready.)
*****************************************************************************/
void SimpleVent::initialize()
{
  if (initialized) {
    return;
  }
  if (check_configuration()) {
    initialized = true;
  }
}

/*****************************************************************************
update
Purpose:(For a vent actively venting -- i.e. producing a force, not an
         impulse -- check the current time against the stop time, and
         recompute the force)
*****************************************************************************/
void SimpleVent::update()
{
  if (!venting) {
    return;
  }

  if ( active && (indefinite_duration || dyn_time < stop_time)) {
    update_force();
  }
  else {
    stop_venting();
  }
}

/*************************************************************************
start_venting
Purpose:(If in dynamic mode, marks the vent as venting and computes the
         stop time)
*************************************************************************/
bool SimpleVent::start_venting()
{
  if (!active) {
    if (!quiet_message_start_inactive) {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid request\n",
        "Cannot start inactive vent '", name, "'.\n");
      }
    return false;
  }

  if (!initialized) { // all initialization checks have passed.
    CMLMessage::error(
      __FILE__,__LINE__,"Incomplete initialization\n",
      "Vent '", name, "' was instructed to start venting but has not completed its "
      "initialization sequence.\nVenting failed.");
    return false;
  }

  // Check that all parameters are set; some may have been manually unset
  // and the reconfiguration insufficiently complete.
  if (apply_as_impulse) {
    if (!impulse_set) {
      CMLMessage::error(
      __FILE__,__LINE__,"Incomplete initialization\n",
      "Vent '", name, "' was instructed to start impulsive venting but has not been "
      "completely configured.\nConfiguration flags are:\n"
      "direction        :  ", direction_set, "\nimpulse magnitude: ", impulse_mag_set, "\nimpulse vector   : ", impulse_set, "\n"
      "Venting failed\n.");
      return false;
    }
    // else: Nothing more to check here, apply defined impulse.
  }

  else{ // Dynamic-mode, check that force vector has been set.
    if ( !force_set) {
      CMLMessage::error(
        __FILE__,__LINE__,"Incomplete initialization\n",
        "Vent '", name, "' was instructed to start dynamic venting but has not been "
        "completely configured.\nConfiguration flags are:\n"
        "direction        :  ", direction_set, "\nforce magnitude: ", force_mag_set, "\nforce vector   : ", force_set, "\n"
        "Venting failed\n.");
      return false;
    }
    //else:
    // In dynamic mode with specified duration, we need to
    // track the clocks to know when to switch the vent off again.
    // If there is no duration set, then open the vent indefinitely.
    // If the vent has not been deliberately set to use an indefinite duration,
    // send a warning that the setting is being applied.
    if (!duration_set && !indefinite_duration) {
      if (!indefinite_duration_warning_sent) {
        CMLMessage::warn(
          __FILE__,__LINE__,"Incomplete configuration\n",
          "Vent starting in dynamic (force) mode without a duration.\n"
          "Vent '", name, "' will be opened indefinitely (until the\n"
          "stop-venting method is executed).\n"
          "This message will only be sent once (per vent).\n");
        indefinite_duration_warning_sent = true;
      }
      indefinite_duration = true;
    }
    stop_time = dyn_time + duration;
    start_time = dyn_time;
    venting = true;
  }
  return true;
}

/*****************************************************************************
stop_venting
Purpose:(Switch vent off -- note - this really only applies in dynamic mode
         because an impulsive mode results in an instantaneous action so there
         is nothing to switch off.)
*****************************************************************************/
void SimpleVent::stop_venting()
{
  venting = false;
}

/*****************************************************************************
check_configuration
Purpose:(Checks all parameters have been set.)
*****************************************************************************/
bool SimpleVent::check_configuration()
{
  if (direction_set) {
    if (force_set || impulse_set) {
      CMLMessage::warn(
        __FILE__,__LINE__,"Overconstrained configuration\n",
        "Vent '", name, "' has multiple directions specified, caused by specifying\n"
        "the direction and either (or both) the force-vector and "
        "impulse-vector.\n"
        "Using the direction specified in the set_direction method:\n"
        "[", direction[0], ", ", direction[1], ", ", direction[2], "]\n");
    }
    // else just use direction without additional warning.
  }
  else if (force_set) {
    set_direction(force);
    if ( impulse_set) {
      CMLMessage::warn(
        __FILE__,__LINE__,"Overconstrained configuration\n",
        "Vent '", name, "' has two directions specified, caused by specifying\n"
        "the force vector nd the impulse vector.\n"
        "Using the direction specified in the set_force method:\n"
        "[", direction[0], ", ", direction[1], ", ", direction[2], "].\n");
    }
  }
  else if (impulse_set) { // and not direction and not force
    set_direction(impulse);
  }
  else {
    CMLMessage::error(
      __FILE__,__LINE__,"Incomplete configuration\n",
      "Vent '", name, "' has no direction information.\n"
      "Initialization failed.\nVent is not usable.\n");
    return false;
  }

  // Extract magnitudes from vectors if necessary
  if (force_set) {
    if (force_mag_set) {
      CMLMessage::warn(
        __FILE__,__LINE__,"Overconstrained configuration\n",
        "Vent '", name, "' has two force specifications, caused by specifying the\n"
        "force-vector and the combination (force-magnitude and direction).\n"
        "Using the combined values of the force-magnitude\n"
        "and the direction to override the specified force vector.\n");
      // override action taken below in "if (force_mag_set)" block.
    }
    else {
      force_mag = jeod::Vector3::vmag(force);
      force_mag_set = true;
    }
  }
  if (impulse_set) {
    if (impulse_mag_set) {
      CMLMessage::warn(
        __FILE__,__LINE__,"Overconstrained configuration\n",
        "Vent '", name, "' has two impulse specifications; using the combined values\n"
        "of the impulse-magnitude (as specified) and the direction to \n"
        "override the specified impulse vector.\n");
      // override action taken below in "if (impulse_mag_set)" block.
    }
    else {
      impulse_mag = jeod::Vector3::vmag(impulse);
      impulse_mag_set = true;
    }
  }

  // Resolve force_mag * duration = impulse_mag
  if (duration_set) {
    if (force_mag_set) {
      if (impulse_mag_set) {
        if(apply_as_impulse) {
           set_duration_internal(); // see documentation; used only to resolve
                                    // a potential conflict.
        }
        else {
          double impulse_mag_ = force_mag * duration;
          CMLMessage::warn(
            __FILE__,__LINE__,"Overconstrained configuration\n",
            "Vent '", name, "' has force-magnitude, impulse-magnitude, and\n"
            "duration all specified.\n"
            "Using force-magnitude and duration to reset\n"
            "impulse-magnitude from ", impulse_mag, " to ", impulse_mag_, ".\n");
          user_set_impulse = impulse_mag = impulse_mag_;
          impulse_mag_set = true;
        }
      }
      else { // impulse-mag not set, set it
        user_set_impulse = impulse_mag = force_mag * duration;
        impulse_mag_set = true;
      }
    }
    else if (impulse_mag_set) { // and force-mag not set
      force_mag = impulse_mag / duration; // Duration cannot be set to 0
      force_mag_set = true;
    }
    // else -- neither force-mag nor impulse-mag set -- initialization fails
    // below, either because there is no specified force (in dynamic mode) or
    // no specified impulse (in impulse mode).
  }

  // Check for valid configuration and calculate vectors from direction and
  // magnitude.
  if (force_mag_set) {
    // Note -- direction may have been reset with set_direction since
    // force was set, and direction takes precedent (see top of this
    // method). This step may be redundant.
    jeod::Vector3::scale( direction,
                          force_mag,
                          force);
    force_set = true;
  }
  else if (!apply_as_impulse) {
    CMLMessage::error(
      __FILE__,__LINE__,"Incomplete configuration\n",
      "Vent '", name, "' configured to apply a force but the force has not been\n"
      "defined and cannot be determined.\n"
      "Initialization failed.\nVent is not usable.\n");
    return false;
  }
  if (impulse_mag_set) {
    jeod::Vector3::scale( direction,
                          impulse_mag,
                          impulse);
    impulse_set = true;
  }
  else if (apply_as_impulse) {
    CMLMessage::error(
      __FILE__,__LINE__,"Incomplete configuration\n",
      "Vent '", name, "' was configured to apply an impulse but the impulse\n"
      "has not been defined and cannot be determined.\n"
      "Initialization failed.\nVent is not usable.\n");
    return false;
  }

  user_set_impulse = impulse_mag;
  user_set_duration = duration;
  return true; // Initialization succeeded
}

/*****************************************************************************
set_impulse_internal
Purpose:(Computes the impulse; this method is called
         when the force or duration is updated post-initialization.
         In either case, the previously specified impulse (if it exists) is
         invalidated and has to be recalculated.  If the force-magnitude or
         duration have not been specified, this calculation cannot proceed.)
*****************************************************************************/
void SimpleVent::set_impulse_internal()
{
  if (!duration_set || !force_mag_set) {
    if (apply_as_impulse) {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid impulse reset.\n",
        "Vent '", name, "': In order to change the applied impulse by adjusting\n"
        "either force or duration, the other component must be known.\n"
        "Current configuration:\n"
        "force set: ", duration_set, "\nduration set: ", force_mag, "\nforce magnitude: ", force_mag, " N\n"
        "duration: ", duration, " s\n"
        "Impulse being retained at its current value:\nimpulse mag: ", impulse_mag, " N*s\n");
    }
    else {
      // don't actually need the impulse in dynamic mode,
      // simply mark it as being invalid.
      impulse_mag_set = false;
      impulse_set = false;
    }
    return;
  }

  impulse_mag = force_mag * duration;
  jeod::Vector3::scale( force,
                        duration,
                        impulse);
  impulse_set = true;
  impulse_mag_set = true;
  if (apply_as_impulse) {
    user_set_impulse = impulse_mag;
  }
}

/*****************************************************************************
set_duration_internal
Purpose:(Computes the duration of the burn given the specified impulse and
         force magnitudes)
*****************************************************************************/
void SimpleVent::set_duration_internal()
{
  if ( force_mag_set && !MathUtils::is_near_equal(force_mag, 0.0)) {
    if (impulse_mag_set) { // 0.0 can be a legitimate setting.
      duration = impulse_mag / force_mag;
      duration_set = true;
      if (!apply_as_impulse) {
        // see description on user_set_duration in header file.
        user_set_duration = duration;
      }
    }
    else if (duration_set) { // impulse-mag not set, duration is invalid.
      CMLMessage::error( __FILE__,__LINE__,
        "Cannot determine vent duration\n",
        "Vent '", name, "': Tried to find duration but impulse is unknown.\n"
        "Duration is no longer valid.\n");
      duration_set = false;
    }
    else { // duration and impulse unset; cannot compute duration
      CMLMessage::error( __FILE__,__LINE__,
        "Cannot determine vent duration\n",
        "Vent '", name, "': Tried to find duration but impulse is unknown.\n"
        "Duration remains unset.\n");
    }
  }
  else if (impulse_mag_set) { // force not set (to valid value) but impulse is
    if (!apply_as_impulse) {
      CMLMessage::error(__FILE__, __LINE__,
        "Cannot determine vent duration\n",
        "Vent '", name, "': Tried to find duration from finite impulse and zero "
        "or unassigned force.\nDuration is no longer valid.\n");
    }
    // else applying as an impulse, appropriate to have a zero duration in
    // this case, no error message needed. Just be sure that duration is
    // unset. So for both cases:
    duration = 0.0;
    duration_set = false;
  }
  else { // Neither force nor impulse set correctly.
    CMLMessage::error(__FILE__, __LINE__,
      "Cannot determine vent duration\n",
      "Vent '", name, "': Tried to find duration with zero or unassigned force\n"
      "and unassigned impulse.\nUnsetting duration.\n");
    duration = 0.0;
    duration_set = false;
  }
}


/*****************************************************************************
set_force_internal
Purpose:
  Sets the force-magnitude and force-vector when the duration and
  impulse have been specified and when the model has already been initialized.
*****************************************************************************/
void SimpleVent::set_force_internal()
{
  // sanity check, just in case:
  if (!impulse_mag_set || !duration_set || !initialized) return;

  force_mag =  MathUtils::divide_protected( impulse_mag,
                                            duration,
                                            0,
                                            false);
  // Direction must be known if the model has been initialized
  // (cannot initialize without a direction!)
  jeod::Vector3::scale( direction,
                        force_mag,
                        force);
  force_mag_set = true;
  force_set = true;
}
