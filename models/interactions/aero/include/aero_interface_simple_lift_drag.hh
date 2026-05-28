/*******************************************************************************
PURPOSE:
  (Header file for AeroInterfaceSimpleLiftDrag C++ class)

LIBRARY DEPENDENCY:
  ((../src/aero_interface_simple_lift_drag.cc))

PROGRAMMERS:
  (((Jeremy Rea) (NASA) (March 2025) (Antares) (initial version, based on AeroInterfaceSimple)))
*******************************************************************************/

#ifndef CML_AERO_INTERFACE_SIMPLE_LIFT_DRAG_HH
#define CML_AERO_INTERFACE_SIMPLE_LIFT_DRAG_HH

#include "aero_interface_base.hh"
#include "aero_executive_simple_lift_drag.hh"

/*******************************************************************************
AeroInterfaceSimpleLiftDrag
Purpose:(Provides a simple aerodynamic interface to access the simple
         aero executive exclusively.)
*******************************************************************************/
class AeroInterfaceSimpleLiftDrag : public AeroInterfaceBase
{
public:
  AeroExecutiveSimpleLiftDrag executive; /* (--) The simple executive. */

  AeroInterfaceSimpleLiftDrag( AtmosRelativeState & atmos_rel_state,
                       const double & geodetic_altitude,
                       const double (&T_inrtl_struc)[3][3],
                       const double & mass);
  virtual ~AeroInterfaceSimpleLiftDrag(){};

  virtual void initialize() override;
  virtual void update() override;

private:
  // Make the copy constructor and assignment operator private
  // (and unimplemented) to avoid erroneous copies.
  AeroInterfaceSimpleLiftDrag (const AeroInterfaceSimpleLiftDrag &);
  AeroInterfaceSimpleLiftDrag & operator = (const AeroInterfaceSimpleLiftDrag &);
};
#endif
