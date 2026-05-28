/*******************************TRICK HEADER******************************
PURPOSE: (The RcsJetGroup provides a convenient mechanism for grouping
  like-Rcs-jets together to give them identical behavioral configurations.
  In the C-based model on which this object is based, the RCSGENERIC model
  had several instances of RCS_MODEL that needed instantiating; this object
  represents a very similar concept to RCS_MODEL.)

LIBRARY DEPENDENCY:
   ((../src/rcs_group.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (April 2017) (Antares)
       (initial object-oriented implementation))
  )
**********************************************************************/

#ifndef RCS_JET_GROUP_HH
#define RCS_JET_GROUP_HH

#include <vector>
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

/*****************************************************************************
RcsGenericModel
Purpose:(Jet models)
*****************************************************************************/
class RcsJetGroup {
 protected:
  double consumption_epsilon; /* (--)
       value for comparing consumption-ratio sum against the value 1.
       The components should sum to 1 +/- consumption_epsilon. */
  const unsigned int & num_prop_components; /* (--)
       reference to the number of propulsion components as defined in
       RcsGeneric.*/

  bool  blow_down ;      /* (--)
      Flag indicating if the thruster blow down model should be used for
      this module, Yes = use model */
 public:
  /****** Inputs ******/
  // Control flags:
  bool  propc_use_isp;      /* (--)
      Determines whether to use Isp or mass-flow for propellant consumption
      calculations. */

  // General inputs
  double signal_delay_time; /* (s)  Delay time from command to start of actuation */
  double on_dead_time;    /* (s)
      Time from when the jet's motor is activated until the thrust is seen.*/
  double off_dead_time;    /* (s)
      Time from when the jet's motor is activated until the thrust starts
      to be reduced. */
  double build_up_time;    /* (s)  Time taken to build up to full thrust */
  double trail_off_time;   /* (s)  Time taken to trail off from full thrust */
  double min_on_time;     /* (s)  Min allowed time between command on and off */
  double min_off_time;    /* (s)
      Min allowed time between burn completion and new burn start */

  // Specialized inputs, not always needed:
  double mixture_ratio ;   /* (--)
      (Ratio) of fuel to oxidizer: fuel/oxidizer.
      Used when RcsGeneric::num_prop_comp = 2 AND
                RcsGeneric::calc_flow_rate = true AND
                blow_down = false AND
                propc_use_isp = false */

  std::vector<double> isp_prop_comp_ratio; /* (--)
      Indicate the ratio (as percentage) of propellant consumption between
      the various propellant component when using ISP method
      Used when (num_prop_components > 1 AND propc_use_isp = true) */


  // Blowdown characteristics:
  std::vector<double> bd_force_coef;  /* (--)
      coefficients for blowdown force calc.
      Used only when blow_down set */
  std::vector<double> bd_isp_coef;   /* (--)
      coefficients for blowdown isp calc.
      Used only when blow_down set */
  double bd_pressure_limit; /* (N/m2)
      limit below which blowdown jets stop functioning.
      Used only when blow_down set */

  /****** Work space ******/
  bool   buffer_flag ;  /* (--)  Flag if buffering of commands is required */
  int    buffer_on_size ; /* (--)  buffer size for on commands */
  int    buffer_off_size; /* (--)  buffer size for off commands */
  double delay_time_on;  /* (s)
      The delay into an rcs cycle before a command is executed = Modulus of
      total_on_delay / cycle time, note this is NOT a user input */
  double delay_time_off; /* (s)
      The delay into an rcs cycle before a command off is executed = Modulus
      of total_off_delay / cycle time, note this is NOT a user input */


  explicit RcsJetGroup( const unsigned int & num_prop_components);
  void initialize (double time_step);
  void set_blow_down( bool blow_down_);
  bool get_blow_down() {return blow_down;};
  unsigned int get_num_prop_components() {return num_prop_components;};

 private:
   // Not implemented:
   RcsJetGroup (const RcsJetGroup& rhs);
   RcsJetGroup & operator = (const RcsJetGroup& rhs);;
};

#endif