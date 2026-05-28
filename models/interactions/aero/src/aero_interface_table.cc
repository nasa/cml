/*******************************  TRICK HEADER  ********************************
PURPOSE:
  (Provide the functionality for the  Aero capabilities when the
   lookup table option is used.)

REFERENCE:
  (The CML aero models, written by Jeremy Rea, Jon Berndt, Sara McNamara,
   Sara Blatz, et, al.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (May 2014) (Antares cleanup) (Adaptation from C-code))
   ((Gary Turner) (OSR) (June 2016) (Antares cleanup) (refactor))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#include <iostream>
#include "../include/aero_interface_table.hh"

/*******************************************************************************
Constructors
*******************************************************************************/
AeroInterfaceTable::AeroInterfaceTable(
    AtmosRelativeState & atmos_rel_state_in,
    const double & geodetic_altitude_in,
    const double * const cg_position_in,
    const double * const true_body_rates_in,
    const double (&T_struc_to_body_in)[3][3])
  :
  AeroInterfaceBaseExtended( atmos_rel_state_in,
                             geodetic_altitude_in,
                             cg_position_in,
                             true_body_rates_in),
  executive( output,
             environment,
             disable_aero_moments,
             disable_aero_damping,
             T_struc_to_body_in)
{
  executive_ptr = & executive;
}

/*******************************************************************************
initialize
Purpose: (Verifies that all pointers etc are set.)
*******************************************************************************/
void
AeroInterfaceTable::initialize()
{
  // enabled and initialized come from AeroInterface from SubscriptionBase
  if (!enabled || initialized) {
    return;
  }

  executive.initialize();
  AeroInterfaceBaseExtended::initialize();
}

/*******************************************************************************
update
Purpose: (The top level function call.)
*******************************************************************************/
void
AeroInterfaceTable::update()
{
  //*****************************************************************
  // Check whether the aero model is enabled
  //*****************************************************************
  if (!active) {
    return;
  }
  executive.update();
}

/*******************************************************************************
activate
Purpose: (Activates the model.)
*******************************************************************************/
void
AeroInterfaceTable::activate()
{
  // This could get messy if it fails because we won't know for sure where it
  // failed. Get a record of the current subscriptions count.
  unsigned int atmos_rel_angles_sub_count =
                        atmos_rel_state.get_angles_subscription_count();
  atmos_rel_state.subscribe_angles_calculation();
  AeroInterfaceBaseExtended::activate();
  // if activation fails, undo.
  if (!active) {
    // if the subscription above has not been removed, remove it.
    // The only ways for activation to fail are:
    //  - already active (currently unreachable)
    //  - executive_ptr = NULL (currently unreachable)
    //  - atmos-rel-state subscription fails
    //  - altitude out of bounds (self-deactivates, which removes the
    //                            angles-calc subscription)
    if (atmos_rel_state.get_angles_subscription_count() >
                                                  atmos_rel_angles_sub_count) {
      atmos_rel_state.unsubscribe_angles_calculation();
    }
    // If the subscription count still doesn't match the original,
    // something else went wrong.
    if (atmos_rel_state.get_angles_subscription_count() !=
                                                  atmos_rel_angles_sub_count) {
      // UNREACHABLE CODE.
      CMLMessage::error(
        __FILE__,__LINE__,"Failed activation\n",
        "The activation of the AeroInterfaceTable failed in the base class.\n"
        "The subscription count to the atmos-rel-state angles calculation has\n"
        "changed in the process. It was ", atmos_rel_angles_sub_count, ", and it is now ", atmos_rel_state.get_angles_subscription_count(), ".\n"
        "Check these values.\nNo further action being taken.\n");
    }

  }
}

/*******************************************************************************
deactivate
Purpose: (Deactivates the model.)
*******************************************************************************/
void
AeroInterfaceTable::deactivate()
{
  if (active) {
    atmos_rel_state.unsubscribe_angles_calculation();
    AeroInterfaceBaseExtended::deactivate();
  }
}