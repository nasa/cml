/*******************************TRICK HEADER************************************
PURPOSE: (Standalone front-end to the aerodynamics data tables.
          The AeroTableSet is a specific case of the more generic
          TableLookupSet, with the intent specifically focused on
          data tables representing aerodynamics.
          The AeroTableSetBase provides the actual content,and is
          inherited into AeroTableSet to provide the interface with the
          AeroExecutiveTable.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Dec 2015) (Antares) (initial version))
   ((Gary Turner) (OSR) (June 2016) (Antares) (Refactor))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#ifndef CML_AERO_TABLE_SET_HH
#define CML_AERO_TABLE_SET_HH
#include<string>
#include <utility>

#include "aero_table_set_base.hh"
#include "aero_executive_table.hh"

class AeroTableSet: public AeroTableSetBase
{
public:
  AeroTableSet( std::string name_in,
                AeroExecutiveTable & exec_table)
  :
  AeroTableSetBase( std::move(name_in),
                    exec_table.coefficients,
                    exec_table.uncertainty)
  {
    // AeroExecutiveTable determines which table-set to use, so turn all
    // table-sets off initially.
    active = false;
    // Add this table to the executive list for later selection:
    exec_table.add_table(this);
  }
  AeroTableSet (const AeroTableSet &) = delete;
  AeroTableSet & operator = (const AeroTableSet &) = delete;
};
#endif
