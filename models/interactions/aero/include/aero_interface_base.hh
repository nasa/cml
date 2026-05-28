/*******************************************************************************
PURPOSE:
  (Header file for AeroInterfaceBase C++ class)

LIBRARY DEPENDENCY:
  ((../src/aero_interface_base.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2016) (Antares) (new))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#ifndef CML_AERO_INTERFACE_BASE_HH
#define CML_AERO_INTERFACE_BASE_HH

#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "cml/models/dynamics/state_descriptors/atmos_rel_state/include/atmos_relative_state.hh"

#include "aero_interface_output.hh"
#include "aero_executive_base.hh"

/*******************************************************************************
AeroInterfaceBase
Purpose: (Abstract class providing the base for the AeroInterface options.)
*******************************************************************************/
class AeroInterfaceBase : public SubscriptionBase
{
protected:
  /***************************************************************************/
  // References to external models, used as control-inputs
  /***************************************************************************/
  AtmosRelativeState     & atmos_rel_state; /* (--)
       The atmospheric relative state for this vehicle.
       Non-const to allow for subscription management.*/
  const double & geodetic_altitude;     /* (m)
       Geodetic altitude, used for switching the model on/off. */

  /***************************************************************************/
  // Internal variables
  /***************************************************************************/
  AeroExecutiveBase * executive_ptr; /* (--) 
       A pointer to the currently-used executive.*/

public:
  /***************************************************************************/
  // User-settable inputs
  /***************************************************************************/
  bool disable_aero_moments; /* (--) self-explanatory */
  bool disable_aero_damping; /* (--) self-explanatory */
  double max_atmos_altitude; /* (m)  Only activate if at a sensible altitude. */

  /***************************************************************************/
  // Outputs
  /***************************************************************************/
  AeroInterfaceOutput output; /* (--) output of the model. */
public:
  AeroInterfaceBase( AtmosRelativeState & atmos_rel_state_in,
                     const double       & geodetic_altitude);

  virtual ~AeroInterfaceBase() {};

  virtual void update() = 0;

protected:
  virtual void activate();
  virtual void deactivate();

private:
  // Make the copy constructor and assignment operator private
  // (and unimplemented) to avoid erroneous copies
  AeroInterfaceBase (const AeroInterfaceBase &);
  AeroInterfaceBase & operator = (const AeroInterfaceBase &);
};
#endif
