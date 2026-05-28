/*******************************************************************************
PURPOSE:
  (Header file for AeroInterfaceSimple C++ class)

LIBRARY DEPENDENCY:
  ((../src/aero_interface_simple.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (March 2015) (Antares) (initial version))
   ((Gary Turner) (OSR) (June 2016)  (Antares) (refactor))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#ifndef CML_AERO_INTERFACE_SIMPLE_HH
#define CML_AERO_INTERFACE_SIMPLE_HH

#include "aero_interface_base.hh"
#include "aero_executive_simple.hh"

/*******************************************************************************
AeroInterfaceSimple
Purpose:(Provides a simple aerodynamic interface to access the simple
         aero executive exclusively.)
*******************************************************************************/
class AeroInterfaceSimple : public AeroInterfaceBase
{
public:
  AeroExecutiveSimple executive; /* (--) The simple executive. */

  AeroInterfaceSimple( AtmosRelativeState & atmos_rel_state,
                       const double & geodetic_altitude,
                       const double (&T_inrtl_struc)[3][3],
                       const double & mass);
  virtual ~AeroInterfaceSimple(){};

  virtual void initialize();
  virtual void update() override;

private:
  // Make the copy constructor and assignment operator private
  // (and unimplemented) to avoid erroneous copies.
  AeroInterfaceSimple (const AeroInterfaceSimple &);
  AeroInterfaceSimple & operator = (const AeroInterfaceSimple &);
};
#endif
