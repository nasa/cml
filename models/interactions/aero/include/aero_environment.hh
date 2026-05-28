/*******************************************************************************
PURPOSE:
  (Define interface between AeroExec and environment.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2016) (Antares) (New))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#ifndef CML_AERO_ENVIRONMENT_HH
#define CML_AERO_ENVIRONMENT_HH

#include <cstring>  // NULL
#include "cml/models/utilities/cml_message/include/cml_message.hh"

/*******************************************************************************
AeroEnvironment
Purpose: (References the environment variables;
          used only by the table-lookup and API.)
*******************************************************************************/
class AeroEnvironment
{
protected:
  /***************************************************************************/
  // References to external models, used as inputs.
  /***************************************************************************/
  const double & dynamic_pressure;      /* (N/m2)  Dynamic pressure */
  const double & Mach_number;           /* (--)    Current Mach number */
  const double & total_angle_of_attack; /* (rad)   Current total angle of attack */
  const double & angle_of_attack;       /* (rad)   Current angle of attack */
  const double & angle_of_sideslip;     /* (rad)   Current angle of sideslip */
  const double & phi_roll;              /* (rad)   Current phi roll angle */
  const double & free_stream_vel_mag;   /* (m/s)   Magnitude of freestream vel */
  const double * const cg_position;     /* (m)     center-of-gravity in body axes */
  const double * const true_body_rates; /* (rad/s) body-rates wrt inrtl in body */

public:
  AeroEnvironment( const double & dynamic_pressure_in,
                   const double & Mach_number_in,
                   const double & total_angle_of_attack_in,
                   const double & angle_of_attack_in,
                   const double & angle_of_sideslip_in,
                   const double & phi_roll_in,
                   const double & free_stream_vel_mag_in,
                   const double * const cg_position_in,
                   const double * const true_body_rates_in)
    :
    dynamic_pressure( dynamic_pressure_in),
    Mach_number( Mach_number_in),
    total_angle_of_attack( total_angle_of_attack_in),
    angle_of_attack( angle_of_attack_in),
    angle_of_sideslip( angle_of_sideslip_in),
    phi_roll( phi_roll_in),
    free_stream_vel_mag( free_stream_vel_mag_in),
    cg_position( cg_position_in),
    true_body_rates( true_body_rates_in)
  {
    if (cg_position == NULL) {
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid construction of Table/API interface\n",
        "The cg_position pointer was not specified.\n"
        "It is required.\n");
    }
    if (true_body_rates == NULL) {
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid construction of Table/API interface\n",
        "The true_body_rates pointer was not specified.\n"
        "It is required.\n");
    }
  };

  double get_mach() {return Mach_number;};
  double get_dynamic_pressure() { return dynamic_pressure;};
  double get_total_angle_of_attack() { return total_angle_of_attack;};
  double get_angle_of_attack() { return angle_of_attack;};
  double get_angle_of_sideslip() { return angle_of_sideslip;};
  double get_phi_roll() { return phi_roll;};
  double get_free_stream_vel_mag() { return free_stream_vel_mag;};
  const double * get_cg_position() { return cg_position;};
  const double * get_true_body_rates() { return true_body_rates;};

private:
  // Make the copy constructor and assignment operator private
  // (and unimplemented) to avoid erroneous copies.
  AeroEnvironment (const AeroEnvironment &);
  AeroEnvironment & operator = (const AeroEnvironment &);
};
#endif