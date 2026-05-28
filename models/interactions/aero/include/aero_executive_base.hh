/*******************************************************************************
PURPOSE:
  (Define baseline executive capability. Pure abstract class.)

PROGRAMMERS:
  (((Jeremy Rea) (NASA) (May 2005) (ARES) (Initial implementation for ARES))
   ((Jeremy Rea) (NASA) (July 2005) (ARES) (Modified for ARES V5.1.1))
   ((Jeremy Rea) (NASA) (October 2005) (ARES) (Added PHI_ROLL option))
   ((Jeremy Rea) (NASA) (October 2005) (ARES) (Added CG/MRC frame options))
   ((Jeremy Rea) (NASA) (November 2005) (ARES) (Aerodynamic executive))
   ((Jeremy Rea) (NASA) (January 2006) (ARES) (Clean-up the code to match documentation))
   ((Jeremy Rea) (NASA) (February 2006) (ARES) (Add aerodynamic damping as table input))
   ((Jeremy Rea) (NASA) (March 2006) (ARES) (Add THRUST_COEF option))
   ((Jeremy Rea) (NASA) (March 2006) (ARES) (Add DELTA_*_FACTOR options))
   ((Jeremy Rea) (NASA) (April 2006) (ARES) (Add body angular rate options and altitude options))
   ((Ryan Whitley) (NASA) (May 2006) (CEV) (RDLaa08887))
   ((Jeremy Rea) (NASA) (June 2006) (ARES) (RDLaa08919: Add base force option))
   ((Jeremy Rea) (NASA) (June 2006) (ARES) (RDLaa08929: Various small fixes to the aero model))
   ((Nathan Payne) (NASA) (July 2006) (ARES) (Add acd_flag to AERO_CURRENT structure))
   ((Jeremy Rea) (NASA) (July 2006) (ARES) (RDLaa08984: Add drag, side, and lift force computations))
   ((Jeremy Rea) (NASA) (July 2006) (ARES) (RDLaa09002: Add table-driven aero uncertainties))
   ((Jeremy Rea) (NASA) (September 2006) (ARES) (RDLaa09141: Reorganization of some structures to mimic CEV aero executive))
   ((Jeremy Rea) (NASA) (January 2007) (CEV) (RDLaa09504: Implement CEV aerodata subroutine V0.2 [mean_free_path]))
   ((Sara Blatz) (NASA) (September 2008) (CEV) (RDLaa10710: Implement CEV aerodata subroutine V0.51))
   ((Gary Turner) (OSR) (January 2016) (Antares) (Cleanup))
   ((Gary Turner) (OSR) (June 2016) (Antares) (Refactor, removed AeroCurrent))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#ifndef CML_AERO_EXECUTIVE_BASE_HH
#define CML_AERO_EXECUTIVE_BASE_HH

#include <cstring>  // NULL
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

#include "aero_interface_output.hh"

/*****************************************************************************
AeroExecutiveBase
Purpose: Baseline Aero Executive, intended to be common across all aero
         interfaces
*****************************************************************************/
class AeroExecutiveBase : public SubscriptionBase
{
protected:
  /***************************************************************************/
  // References to external model data
  /***************************************************************************/
  AeroInterfaceOutput & output; /* (--)
     Reference to the instance in the interface that holds the model outputs.*/

public:
  /***************************************************************************/
  // User-specified inputs
  /***************************************************************************/
  double Aref;    /* (m2)   Current aerodymamic reference area */
  double Lref;    /* (m)    Current aerodynamic reference length */

  // Constructor
  AeroExecutiveBase( AeroInterfaceOutput & output_ref)
    :
    output( output_ref),
    Aref(0.0),
    Lref(0.0)
  {
    subscribe_name = "AeroExecutiveBase:";
  }

  virtual void deactivate() {
    output.zero_everything();
    SubscriptionBase::deactivate();
  }

  virtual void update() = 0;

private:
  // Make the copy constructor and assignment operator private
  // (and unimplemented) to avoid erroneous copies
  AeroExecutiveBase (const AeroExecutiveBase &);
  AeroExecutiveBase & operator= (const AeroExecutiveBase &);
};
#endif