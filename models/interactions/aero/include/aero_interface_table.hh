/*******************************************************************************
PURPOSE:
  (Header file for AeroInterfaceTable C++ class.
   This class is the front-end to the AeroExecutiveTable class.
   It does little else.)

LIBRARY DEPENDENCY:
  ((../src/aero_interface_table.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (May 2014) (Antares) (new))
   ((Gary Turner) (OSR) (June 2016) (Antares) (refactor))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#ifndef CML_AERO_INTERFACE_TABLE_HH
#define CML_AERO_INTERFACE_TABLE_HH

#include "aero_interface_base_extended.hh"
#include "aero_coefficients.hh"
#include "aero_executive_table.hh"

/*******************************************************************************
AeroInterfaceTable
Purpose: (Provides the Table-look-up version of the Aero model.)
*******************************************************************************/
class AeroInterfaceTable : public AeroInterfaceBaseExtended
{
public:
  /***************************************************************************/
  // Input/output - Some coefficients are input from the table-lookup, others
  //                are calculated from the input values.
  /***************************************************************************/
  AeroExecutiveTable  executive; /* (--) The executable component. */

public:
  AeroInterfaceTable( AtmosRelativeState & atmos_rel_state_in,
                      const double & geodetic_altitude,
                      const double * const cg_position,
                      const double * const true_body_rates,
                      const double (&T_struc_to_body_in)[3][3]);
  virtual ~AeroInterfaceTable(){};

  virtual void initialize();
  virtual void update() override;
protected:
  virtual void activate() override;
  virtual void deactivate() override;

private:
  // Make the copy constructor and assignment operator private
  // (and unimplemented) to avoid erroneous copies.
  AeroInterfaceTable (const AeroInterfaceTable &);
  AeroInterfaceTable & operator= (const AeroInterfaceTable &);
};
#endif
