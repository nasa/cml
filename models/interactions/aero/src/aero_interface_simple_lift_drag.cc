/*******************************TRICK HEADER************************************
PURPOSE: ( Provides the simple interface, exclusive to simple
           non-rotating aerodynamic lift/drag model)

PROGRAMMERS:
  (((Jeremy Rea) (NASA) (March 2025) (Antares) (initial version, based on AeroInterfaceSimple)))
*******************************************************************************/

#include <cstring>  // NULL
#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/aero_interface_simple_lift_drag.hh"

/*******************************************************************************
Constructor:
*******************************************************************************/
AeroInterfaceSimpleLiftDrag::AeroInterfaceSimpleLiftDrag(
    AtmosRelativeState & atmos_rel_state_in,
    const double & geodetic_altitude_in,
    const double (&T_inrtl_struc)[3][3],
    const double & mass)
  :
  AeroInterfaceBase( atmos_rel_state_in,
                     geodetic_altitude_in),
  executive( output,
             atmos_rel_state.dynamic_pressure,
             atmos_rel_state.T_inrtl_traj,
             T_inrtl_struc,
             mass)
{
  executive_ptr = &executive;
}

/*******************************************************************************
initialize
Purpose: (Verifies that all pointers etc are set.)
*******************************************************************************/
void
AeroInterfaceSimpleLiftDrag::initialize()
{
  // enabled and initialized come from AeroInterface from SubscriptionBase
  if (!enabled || initialized) {
    return;
  }
  // Initialize dependencies and then push up the inheritance chain.
  executive.initialize();
  AeroInterfaceBase::initialize();
}

/*******************************************************************************
update
Purpose: (The top level function call.)
*******************************************************************************/
void
AeroInterfaceSimpleLiftDrag::update()
{
  //*****************************************************************
  // Check whether the aero model is enabled
  //*****************************************************************
  if (!active) {
    return;
  }
  if (atmos_rel_state.get_angles_subscription_count() == 0) {
    atmos_rel_state.compute_T_inrtl_traj();
  }
  executive.update();
}
