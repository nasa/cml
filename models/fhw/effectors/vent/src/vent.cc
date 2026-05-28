/*******************************TRICK HEADER******************************
PURPOSE: (Generic vent model with mass depletion)

PROGRAMMERS:
  (((Daniel Ghan) (OSR) (Mar 2020) (Antares) (initial version)))
************************************************************************/

#include "jeod/models/utils/math/include/vector3.hh"

#include "../include/vent.hh"

/************************************************************************
Constructor
************************************************************************/
Vent::Vent(
     const double& dyn_time,
     DynamicMassBody& tank_in)
  :
  SimpleVent( dyn_time),
  tank(tank_in),
  flowrate(0.0),
  exhaust_speed(0.0),
  prev_time(0.0),
  flowrate_set(false),
  exhaust_set(false)
{}

/*****************************************************************************
initialize inherited from SimpleVent
*****************************************************************************/

/*****************************************************************************
check_configuration
Purpose:(Checks all parameters have been set)
*****************************************************************************/
bool Vent::check_configuration()
{
  // By setting flowrate and exhaust-speed, force-magnitude could be implied.
  // Before calling SimpleVent::check_configuration, we need to process this
  // combination:
  if (flowrate_set && exhaust_set) {
    if (force_mag_set || force_set) {
      CMLMessage::warn(__FILE__, __LINE__,
        "Overconstrained configuration\n",
        "Vent '", name, "': Flowrate, exhaust speed, and force-magnitude (either\n"
        "directly or indirectly via force-vector) have all been\n"
        "specified. Force magnitude will be overwritten by\n"
        "flowrate * exhaust speed.\n");
    }
    set_force_magnitude( flowrate * exhaust_speed);
  }

  // if the simple-vent configuration fails, this one also fails.
  if (!SimpleVent::check_configuration()) {
    return false;
  }

  // flowrate AND exhaust set already covered
  // flowrate XOR exhaust set: set the unset one using force-mag, if available
  if ((force_mag_set) && (flowrate_set != exhaust_set)) {
    set_exhaust_flowrate_from_force(exhaust_set);
  }

  // flowrate NOR exhaust set: one of these checks will fail
  if (apply_as_impulse) {
    // Impulse is set; otherwise SimpleVent::check_configuration would have
    // failed.
    if (!exhaust_set) {
      CMLMessage::error(
        __FILE__,__LINE__,"Incomplete configuration\n",
        "Vent '", name, "' was configured to apply an impulse but the exhaust speed\n"
        "has not been defined and cannot be determined.\n"
        "Initialization failed.\nVent is not usable.\n");
      return false;
    }
  }
  else if (!flowrate_set) { // and !apply_as_impulse
    // Force is set; otherwise SimpleVent:check_configuration would have failed.
      CMLMessage::error(
        __FILE__,__LINE__,"Incomplete configuration\n",
        "Vent '", name, "' configured to apply a force but the flowrate\n"
        "has not been defined and cannot be determined.\n"
        "Initialization failed.\nVent is not usable.\n");
    return false;
  }
  return true;
}

/*****************************************************************************
update
Purpose:(Top level function call, called from VentSet)
*****************************************************************************/
void Vent::update()
{
  if (!venting) {
    return;
  }

  /* NOTE - mass-demand is placed on the dynamic-mass body in the cycle
            *following* the integration of the force (when operating in
            dynamic mode).
     Because the model is provided with a mass-derivative rate -- not a
     discrete mass demand -- it must use a time step to convert that
     derivative into a mass demand.  Anticipating that first time step
     is problematic, so it is easier (and numerically slightly superior)
     to place the discrete mass demand post-integration rather than
     pre-integration.

     Consequently, the first step of this method is to place the mass
     demand for the mass just vented in the most recent cycle.

     Note that on the first pass when a vent is just opened, this
     value will be 0 because dyn_time and prev_time are identical
     (see Vent::start_venting())
  */
  update_mass_demand();

  /* now check 3 items to confirm whether an open vent remains venting:
    - the active flag allows detection of any forced-shutdowns
      implemented by deactivating a vent (e.g. simulating a system-failure)
    - the time allows for scheduled closing of a vent
    - the available mass allows for stopping a vent when it has no more mass
      to vent.
  */
  double available_mass = tank.dynamic_properties.consumable_mass -
                          tank.dynamic_properties.mass_consumed_step;
  if ( active &&
       (indefinite_duration || dyn_time < stop_time) &&
       available_mass > 0.0) {
    // NOTE - update_force() does nothing in the base Vent class
    //        because the force is assumed constant.  This line
    //        supports derivative-implementations that may have
    //        time-dependencies built in.
    update_force();
  }
  // if any of those checks fails, the vent should be stopped:
  else {
    stop_venting();
  }
}

/*****************************************************************************
use_impulse_mode
Purpose:(Switches between impulse and dynamic modes)
*****************************************************************************/
void Vent::use_impulse_mode(bool mode)
{
  // if already in this mode, do nothing.
  if (mode == apply_as_impulse) {
    return;
  }

  SimpleVent::use_impulse_mode( mode);
  if ( mode) {
    if (initialized && !exhaust_set) {
      CMLMessage::error(
        __FILE__,__LINE__,"Incomplete configuration\n",
        "Vent '", name, "': Cannot transition to impulsive mode because the\n"
        "exhaust speed has not been set.\n");
      apply_as_impulse = false;
    }
  }
  else {
    if (initialized && !flowrate_set) {
      CMLMessage::error(
        __FILE__,__LINE__,"Incomplete configuration\n",
        "Vent '", name, "': Cannot transition to dynamic (force) mode because the\n"
        "flowrate has not been set.\n");
      apply_as_impulse = true;
    }
  }
}

/************************************************************************
set_force_magnitude
Purpose:(SimpleVent method sets the force from a force magnitude and a
         previously specified direction; also sets the exhaust-speed and
         flowrate to be compatible with each other given the magnitude of
         the force.)
************************************************************************/
void Vent::set_force_magnitude(double force_, bool hold_impulse)
{
  SimpleVent::set_force_magnitude(force_, hold_impulse);
  if (initialized) {
    set_exhaust_flowrate_from_force();
  }
}
/************************************************************************
set_force_vector
Purpose:(SimpleVent method sets the force magnitude and direction from a
         force vector; also sets the exhaust-speed and flowrate to be
         compatible with each other given the magnitude of the force.)
************************************************************************/
void Vent::set_force_vector(double force_[3], bool hold_impulse)
{
  SimpleVent::set_force_vector(force_, hold_impulse);
  if (initialized) {
    set_exhaust_flowrate_from_force();
  }
}

/*****************************************************************************
set_flowrate
Purpose:(Sets the mass flowrate.  Note -- if in impulse-mode, this does not
         affect the impulse.)
*****************************************************************************/
void Vent::set_flowrate( double flowrate_, bool hold_exhaust)
{
  // Use an invalid setting as a proxy for unsetting the flowrate.
  if (flowrate_ <= 0.0) {
    if (initialized && !apply_as_impulse) {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid setting\n",
        "Vent '", name, "' is set to dynamic mode, which requires a valid flowrate\n"
        "to compute the mass depletion.\nCannot unset the flowrate while "
        "operating in dynamic mode.\n");
      return;
    }
    flowrate = 0.0;
    flowrate_set = false;
  }
  else {
    flowrate = flowrate_;
    flowrate_set = true;
  }

  if (initialized) {
    if (hold_exhaust) { // default behavior
      if (exhaust_set) {
        // If the flowrate does not affect the exhaust velocity,
        // the force would then linearly scale with the flowrate.
        // If in impulse mode and impulse-magnitude is known, hold it and
        // recompute the duration, otherwise hold the duration and recompute
        // the impulse.
        SimpleVent::set_force_magnitude( flowrate * exhaust_speed,
                                         apply_as_impulse && impulse_mag_set);
      }
    }
    else  {
      set_exhaust_flowrate_from_force(false);
    }
  }
}

/*************************************************************************
set_exhaust_speed
Purpose:(Sets the vent's exhaust speed.
         If operating in dynamic (force) mode, this will modify the force
         applied but does not affect the duration of the venting.
         If operating in impulse mode, changing the exhaust speed will not
         affect the impulse but will affect the flowrate.)
*************************************************************************/
void Vent::set_exhaust_speed(double exhaust_speed_, bool hold_flowrate)
{
  // Use an invalid setting as a proxy for unsetting the exhaust speed.
  if (exhaust_speed_ <= 0.0) {
    if (initialized && apply_as_impulse) {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid setting\n",
        "Vent '", name, "' is set to impulsive mode, which requires a valid \n"
        "exhaust-speed to compute the mass-loss.\n"
        "Cannot unset the exhaust speed while operating in impulsive mode.\n");
      return;
    }
    exhaust_speed = 0.0;
    exhaust_set = false;
  }
  else {
    exhaust_speed = exhaust_speed_;
    exhaust_set = true;
  }

  if (initialized) {
    if(hold_flowrate) { // default behavior
      if (flowrate_set) {
        // It is assumed that adjusting the exhaust does not affect the
        // flowrate; the force would then linearly scale with the exhaust speed.
        // If in impulse mode and impulse-magnitude is known, hold it and
        // recompute the duration, otherwise hold the duration and recompute
        // the impulse.
      SimpleVent::set_force_magnitude( flowrate * exhaust_speed,
                                       apply_as_impulse && impulse_mag_set);
      }
    }
    else {
      // adjust flowrate and delta-mass
      set_exhaust_flowrate_from_force(true);
    }
  }
}

/************************************************************************
set_force_internal
Purpose:(SimpleVent method sets the force magnitude from impulse-magnitude
         and duration, and force-vector from force-magnitude and
         direction.
         This method then also modifies the flowrate / exhaust-speed to
         match the new force magnitude.
************************************************************************/
void Vent::set_force_internal()
{
  SimpleVent::set_force_internal();
  if (initialized && force_mag_set) {
    set_exhaust_flowrate_from_force();
  }
}

/************************************************************************
start_venting
Purpose:(If in dynamic mode, marks the vent as venting and computes the
         stop time. If in impulse mode, makes sure the impulse does not
         require more mass than available.)
************************************************************************/
bool Vent::start_venting()
{
  if (tank.dynamic_properties.consumable_mass <= 0.0) {
    CMLMessage::warn(
      __FILE__, __LINE__, "Empty tank\n",
      "Vent '", name, "': Cannot start venting because tank '",
      tank.name.get_name(), "' is empty!\n");
    return false;
  }

  // exhaust and flowrate can be manually unset, check that they have been
  // reset in that case.
  // TODO Turner 01/2023 Could make an option to support running without
  //                     mass-flow, like a simple-vent.
  if ( ( apply_as_impulse && !exhaust_set) ||
       (!apply_as_impulse && !flowrate_set)  ) {
    CMLMessage::error(
      __FILE__,__LINE__,"Incomplete initialization\n",
      "Vent '", name, "' was instructed to start venting but mass depletion\n"
      "has not been completely configured.\nConfiguration flags are:\n"
      "apply_as_impulse  :", apply_as_impulse, "\nexhaust-speed set :  ", exhaust_set, "\nmass flow-rate set: ", flowrate_set, "\n"
      "Venting failed\n.");
    return false;
    }
    //else:

  prev_time = dyn_time;
  return SimpleVent::start_venting();
}

/*****************************************************************************
check_status
Purpose:(If there is not enough mass to generate the requested impulse, adjusts
         the impulse magnitude.)
*****************************************************************************/
void Vent::check_status()
{
  double available_mass = tank.dynamic_properties.consumable_mass -
                          tank.dynamic_properties.mass_consumed_step;
  if (available_mass * exhaust_speed < impulse_mag) {
      // Scale the impulse magnitude and the mass consumption such that all the
      // mass is used.
      set_impulse_magnitude( available_mass * exhaust_speed);
      CMLMessage::warn(__FILE__, __LINE__,
        "Not enough mass in tank for requested impulse.\n",
        "Tank '", tank.name.get_name(), "' has been emptied and the appropriate impulse of\n",
         impulse_mag, " N*s has been applied.\n");
     tank.dynamic_properties.mass_consumed_step += available_mass;
  }
  else {
    tank.dynamic_properties.mass_consumed_step +=
                                   MathUtils::divide_protected( impulse_mag,
                                                                exhaust_speed,
                                                                available_mass,
                                                                false);
  }
}

/*****************************************************************************
set_exhaust_flowrate_from_force
Purpose:(Computes the exhaust speed or flowrate given a specified force such
         that those three values are consistent.)
*****************************************************************************/
void Vent::set_exhaust_flowrate_from_force( bool hold_exhaust)
{
  // Internal method, only called post-initialization and post-force setting,
  // so force-mag is defined.
  if (hold_exhaust && exhaust_set) {
    flowrate = MathUtils::divide_protected( force_mag,
                                            exhaust_speed,
                                            0.0,
                                            false);
    flowrate_set = true;
  }
  // Otherwise, if not holding the exhaust OR it has not been set yet
  // (regardless of the input argument), compute the exhaust speed.
  else if (flowrate_set) {
    exhaust_speed = MathUtils::divide_protected( force_mag,
                                                 flowrate,
                                                 0.0,
                                                 false);
    exhaust_set = true;
  }
  // Otherwise do nothing -- this case is only reached if using set_flowrate to
  // *unset* the flowrate, in which case we don't want to mess with the
  // exhaust-speed.
}

/*****************************************************************************
stop_venting
Purpose:(Switch vent off -- note - this really only applies in dynamic mode
         because an impulsive mode results in an instantaneous action so there
         is nothing to switch off.)
*****************************************************************************/
void Vent::stop_venting()
{
  // Note -- only update mass if currently venting.
  if (venting) {
    update_mass_demand();
    venting = false;
  }
  // else -- no action needed, it's already off.
}

/*****************************************************************************
update_mass_demand
Purpose:(Places a mass-demand on the tanks for any mass that has been vented
         between the last update and the current dynamic time)
*****************************************************************************/
void Vent::update_mass_demand()
{
  tank.dynamic_properties.mass_consumed_step += flowrate *
                                                      (dyn_time - prev_time);
  prev_time = dyn_time;
}
