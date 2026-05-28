/******************************************************************************
PURPOSE:
  (Trajectory driven model data structure.)

Assumptions:
 (
  There are three model operating modes, each of which creates a different
  response.  In the absence of conclusive validation data, the ultimate
  determination of which mode is most realistic is left up to the user. All 3
  operating modes use the same core algorithm, the input values to the algorithm
  are different per operating mode and remain constant for the run. Low-freq
  perturbations contain no twist component. Perturbations occur at regular
  intervals, which is randomly generated within a specified input range.
  decay_const, an input parameter governing the decay of the twist-sway as the
  simulation nears the end of the pad-alignment phase, must be set by the user
  appropriately to facilitate the desired profile of decay. Perturbations must
  decay completely before Traj prescribed trajectory kicks in.
  The perturbations of the model are assummed to be much much smaller
  than the RocketHeight.  This assumption exists in code via the
  small angle approximation in some computations.  The wind direction is
  intended to remain constant throughout the run. Changing it mid-run is possible
  but may result in a response that is not physically accurate.
 )

Library dependency:
  ((../src/twist_sway.cc))

PROGRAMMERS:
   (
   ((Michael Trevino) (Odyssey) (10/09) (Initial Implementation))
   ((Michael Trevino) (Odyssey) (04/10) (Increased MAX_TIME_TAGS))
   ((Richard Burt) (LMCO) (07/11) (lmbp#981) (Added Stefanie Beaver's
               updates from OrionSim model to prevent indexing through
               entire data file every pass))
   ((Richard Burt) (LMCO) (07/11) (lmbp#983) (Add adjustable time selections))
   ((Richard Burt) (LMCO) (07/11) (lmbp#984) (Add ECEF input option and calcs))
   ((Richard Burt) (LMCO) (07/11) (lmbp#985) (Added option for spherical
                                              interpolation of quaternions))
   ((Brian C. Neumann) (Aerodyne) (Dec 2014) (Updates for Trick 13))
   ((Gary Turner) (OSR) (May 2015) (extensive rewrite))
   )
******************************************************************************/

#ifndef ANTARES_TWIST_SWAY_HH
#define ANTARES_TWIST_SWAY_HH

#include <list>
#include <random> // std::mt19937
#include "jeod/models/utils/quaternion/include/quat.hh"

#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"


/*****************************************************************************
TwistSwayParams
Purpose:(Simple POD class containing the parameters that govern how the
         perturbations are applied and how the stack responds to them)
*****************************************************************************/
class TwistSwayParams
{
 public:
  //Scenario constants
  double RocketHeight;  /* (m)  height of the rocket */
  double wind_direction;/* (rad)  The direction the wind is heading
                                  0- North; pi/2 - East, etc.*/

  //Define the frequency content, long period and short period
  double tau;  /* (s) wind time constant for high-frequency response*/
  double taul; /* (s) wind time constant for low-frequency response */
  double limit_ln_epsilon; /* (--)
         Natural log of the lower limit of resolution
         (default = ln(10^-16) = -37). */

  double Parallel_Motion_Fast;    /* (m)
         Maximum displacement parallel to wind for high-frequency response*/
  double Normal_Motion_Fast;    /* (m)
         Maximum displacement normal to wind for high-frequency response*/
  double Parallel_Motion_Slow;  /* (m)
         Maximum displacement parallel to wind for low-frequency response*/
  double Normal_Motion_Slow;    /* (m)
         Maximum displacement normal to wind for low-frequency response*/

  double Twist_Mag;  /* (rad)  Maximum twist displacement */

  double delta_T1_max;  /* (s)  The maximum time between the fast disturbances */
  double delta_T1_min;  /* (s)  The minimum time between the fast disturbances */
  double delta_T2_max;  /* (s)  The maximum time between the slow disturbances */
  double delta_T2_min;  /* (s)  The minimum time between the slow disturbances */

  double mode_freq_high; /* (Hz)
         The highest possible frequency for the rocket mode */
  double mode_freq_low; /* (Hz)
         The lowest possible frequency for the rocket mode */

  double decay_const;   /* (--)
         A time-related constant governing the decay of the twist-sway as
         the simulation nears the end of the pad-alignment phase.  */

  unsigned int seed; /* (--)  Manually chosen random seed */

  TwistSwayParams();
  virtual ~TwistSwayParams(){};
};

/*****************************************************************************
TwistSwayMagnitudes
Purpose:( Simple POD class containing the parallel, normal, and twist
          magnitudes for a perturbation.  Used internally to the TwistSway
          model only.)
*****************************************************************************/
class TwistSwayMagnitudes
{
 public:
  double parallel;   /* (m) Parallel component */
  double normal;     /* (m) Normal component */
  double twist;      /* (rad) Twist component */
  TwistSwayMagnitudes();
};

/*****************************************************************************
TwistSway
Purpose:(The main class)
*****************************************************************************/
class TwistSway : public SubscriptionBase
{
 public:
  enum PerturbationModel {
    GrowHoldDecay = 0,
    ImpulseDecay = 1,
    GrowDecay = 2
  };
  PerturbationModel perturb_algorithm; /* (--)
                    Choice of algorithm for accumulating perturbations.*/

  TwistSwayParams  params;   /* (--)
       User defined parameters for the twist and sway model. */
  jeod::Quaternion Q_enu_to_ecef;     /* (--)
      left-transform-quat from ENU to ECEF. This must be populated if
      expressing the perturbations in the ECEF frame using update_ecef().
      If only using the ENU outputs, this will be ignored.*/


 // Output

  // Perturbations in ENU frame
  double dp_enu[3];             /* (m)     twist/sway displacement of body
                                           wrt base expressed in ENU */
  double dv_enu[3];             /* (m/s)   twist/sway velocity (ENU) */
  jeod::Quaternion Q_enu_to_ts;       /*(--) left-transformation-quaternion from base
                                       ENU frame to the final TS frame.*/
  double w_ts_wrt_enu_in_enu[3];/* (rad/s) twist/sway angular rates (ENU) */

  // Perturbations in ECEF frame:
  double dp_ecef[3];            /* (m)   twist/sway displacment (ECEF) */
  double dv_ecef[3];            /* (m/s)  twist/sway velocity (ECEF) */
  jeod::Quaternion Q_ecef_to_ts;      /* (--) left-transform-quat from ECEF to TS*/
  double w_ts_wrt_enu_in_ecef[3];/* (rad/s)  twist/sway angular rates (ECEF)*/



 protected:
  std::mt19937 random_generator; /* (--)
      the Mersennes-Twister random number generator.*/

  const double & external_clock; /* (s) reference to an external clock. */
  double previous_external_clock; /* (s) most recently known value of
                                         external_clock */
  double start_time;    /* (s) Value of external_clock at which the twist-sway
                               first runs; this is the time of the first
                               perturbation.*/
  double elapsed_time;  /* (s) Time basis for calculating effects.
                               Computed as external_clock - start_time */
  double end_time;      /* (s) Value of external_clock when twist-sway shall
                               end. */
  double dt; /* (s) time step */

  double T_fast;   /* (s)  The time between the fast disturbances */
  double T_slow;   /* (s)  The time between the slow disturbances */
  double Freqw;    /* (Hz)  The driving frequency */

  double decay_mult;    /* (--) Multiplier to decay combined effect towards
                               end of pad-align. */
  double p_factor_fast; /* (--) Multiplier to decay effect of a single
                               perturbation over 1 "fast" interval, T_fast. */
  double p_factor_slow; /* (--) Multiplier to decay effect of a single
                               perturbation over 1 "slow" interval, T_slow. */

  unsigned int ix_fast_next; /* (--)
      identification of the next multiple of the "fast" interval, T_fast.
      e.g. if T_fast = 2.0 and t = 5.0, ix_fast_next = 3.*/
  unsigned int ix_slow_next; /* (--)
      identification of the next multiple of the "slow" interval, T_slow. */

  std::list< TwistSwayMagnitudes > fast_list; /* (--)
      list of currently acive "fast" perturbations.*/
  std::list< TwistSwayMagnitudes > slow_list; /* (--)
      list of currently acive "slow" perturbations.*/

  TwistSwayMagnitudes fast_mag; /* (--)
      summed effect of all currently active "fast" perturbations. */
  TwistSwayMagnitudes slow_mag; /* (--)
      summed effect of all currently active "slow" perturbations. */

  // raw angles
  double sway_parallel; /* (m)
      the net perturbation representing sway distance parallel to the wind */
  double sway_normal;   /* (m)
      the net perturbation respresenting sway distance perpendicular to wind*/
  double twist_angle;   /* (rad)
      the net perturbation respresenting twisting motion */

  // angles from previous cycle:
  double prev_sway_parallel; /* (--)
      copy of the last known parallel-component of sway,
      used to obtain sway-rate and thereby dv_enu. */
  double prev_sway_normal;   /* (--)
      copy of the last known normal-component of sway,
      used to obtain sway-rate and thereby dv_enu. */
  double prev_dp_up;         /* (--)
      copy of the last known vertical-component of dp_enu,
      used to obtain dv_enu. */
  double prev_twist_sway_angle[3]; /* (--)
      copy of the last known twist_sway-angles, used to obtain attitude-rates.*/

  double copy_wind_direction; /* (rad)
      copy of the last known wind-direction,
      used to identify need to recompute trig functions.*/
  double sin_wind; /* (--) sine of wind direction. */
  double cos_wind; /* (--) cosine of wind direction. */
  bool small_angle_warning_sent; /* (--)
      Controls the warning about the small angle violation. Restricts the
      warning to a single issue,  Default: false. */


public:
  TwistSway(const double & time_ref);
  virtual ~TwistSway(){};

  void initialize(double end_time);
  bool update();
  void update_ecef();

  void set_end_time(double time);
  bool get_active() {return active;};
  TwistSwayMagnitudes & get_fast_mag() {return fast_mag;}
  TwistSwayMagnitudes & get_slow_mag() {return slow_mag;}

 protected:
  void activate();
  void deactivate();
  void new_fast_perturbation();
  void new_slow_perturbation();
  void check_for_active_perturbations();
  void accumulate_perturbations();
  void compute_twist_sway_enu();

 private:
  // Prevent accidental copy and assignment:
  TwistSway (const TwistSway&);
  TwistSway& operator = (const TwistSway&);
};

#endif
