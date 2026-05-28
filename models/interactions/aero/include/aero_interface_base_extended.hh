/*******************************************************************************
PURPOSE:
  (Define baseline interface between AeroExec managers and executors.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2016) (Antares) (New))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#ifndef CML_AERO_INTERFACE_BASE_EXTENDED_HH
#define CML_AERO_INTERFACE_BASE_EXTENDED_HH

#include <cstring>  // NULL
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "aero_interface_base.hh"
#include "aero_environment.hh"

/*******************************************************************************
AeroInterfaceExtended
Purpose:(Common elements for the Table-lookup and Api options.)
*******************************************************************************/
class AeroInterfaceBaseExtended : public AeroInterfaceBase
{
public:
  /***************************************************************************/
  // Additional inputs
  /***************************************************************************/
  AeroEnvironment  environment; /* (--)
          References to numerous environment variables.*/

  AeroInterfaceBaseExtended( AtmosRelativeState & atmos_rel_state_in,
                             const double & geod_alt_in,
                             const double * const cg_position_in,
                             const double * const true_body_rates_in)
    :
    AeroInterfaceBase( atmos_rel_state_in,
                       geod_alt_in),

    environment( atmos_rel_state.dynamic_pressure,
                 atmos_rel_state.mach_number,
                 atmos_rel_state.total_angle_of_attack,
                 atmos_rel_state.angle_of_attack,
                 atmos_rel_state.angle_of_sideslip,
                 atmos_rel_state.phi_roll,
                 atmos_rel_state.free_stream_vel_mag,
                 cg_position_in,
                 true_body_rates_in)
  { };
private:
  // Make the copy constructor and assignment operator private
  // (and unimplemented) to avoid erroneous copies.
  AeroInterfaceBaseExtended (const AeroInterfaceBaseExtended &);
  AeroInterfaceBaseExtended & operator = (const AeroInterfaceBaseExtended &);
};
#endif