/*******************************TRICK HEADER************************************
PURPOSE: ( Provides the simple interface, exclusive to simple
           non-rotating aerodynamic drag model)

PROGRAMMERS:
  (((Gary Turner) (OSR) (March 2015) (Antares) (initial version))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#include <cstring>  // NULL
#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/aero_interface_simple.hh"

/*******************************************************************************
Constructor:
*******************************************************************************/
AeroInterfaceSimple::AeroInterfaceSimple(
    AtmosRelativeState & atmos_rel_state_in,
    const double & geodetic_altitude_in,
    const double (&T_inrtl_struc)[3][3],
    const double & mass)
  :
  AeroInterfaceBase( atmos_rel_state_in,
                     geodetic_altitude_in),
  executive( output,
             atmos_rel_state.dynamic_pressure,
             atmos_rel_state.free_stream_vel_unit,
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
AeroInterfaceSimple::initialize()
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
AeroInterfaceSimple::update()
{
  //*****************************************************************
  // Check whether the aero model is enabled
  //*****************************************************************
  if (!active) {
    return;
  }
  executive.update();
}