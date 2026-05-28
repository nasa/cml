/*******************************************************************************
Purpose:
 (Produce wind generated twist and sway motion)

Programmers:
 (
 ((Rob Gillis) (Emergent) (Dec 2011) (CEV) (Initial version for ground contact model))
 ((Rob Gillis) (Emergent) (Mar 2012) (CEV) (Modified to work properly when being called by a dervative class, and fix other errors))
 ((Brian C. Neumann) (Aerodyne) (Dec 2014) (Updates for Trick 13)))
 ((Gary Turner) (OSR) (May 2015) (extensive rewrite))
 ((Bingquan Wang) (OSR) (Apr 2017) (Disabled the compilation warning of float-point number equality comparison))
 )

*******************************************************************************/
#define _USE_MATH_DEFINES // M_PI
#include <cmath> // M_PI, exp, pow
#include <random> // std::uniform_real_distribution

#include "jeod/models/utils/memory/include/jeod_alloc.hh"
#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"


#include "../include/twist_sway.hh"

/*****************************************************************************
Constructors
*****************************************************************************/
TwistSwayParams::TwistSwayParams()
  :
  RocketHeight(0.0),
  wind_direction(0.0),
  tau(0.0),
  taul(0.0),
  limit_ln_epsilon(-37.0),
  Parallel_Motion_Fast(0.0),
  Normal_Motion_Fast(0.0),
  Parallel_Motion_Slow(0.0),
  Normal_Motion_Slow(0.0),
  Twist_Mag(0.0),
  delta_T1_max(0.0),
  delta_T1_min(0.0),
  delta_T2_max(0.0),
  delta_T2_min(0.0),
  mode_freq_high(0.0),
  mode_freq_low(0.0),
  decay_const(0.0),
  seed(0)
{}

/*****************************************************************************/
TwistSwayMagnitudes::TwistSwayMagnitudes()
  :
  parallel(0.0),
  normal(0.0),
  twist(0.0)
{}

/*****************************************************************************/
TwistSway::TwistSway(
      const double & external_clock_)
  :
  perturb_algorithm(GrowHoldDecay),
  params(),
  Q_enu_to_ecef(),
  dp_enu{0.0, 0.0, 0.0},
  dv_enu{0.0, 0.0, 0.0},
  Q_enu_to_ts(),
  w_ts_wrt_enu_in_enu{0.0, 0.0, 0.0},
  dp_ecef{0.0, 0.0, 0.0},
  dv_ecef{0.0, 0.0, 0.0},
  Q_ecef_to_ts(),
  w_ts_wrt_enu_in_ecef{0.0, 0.0, 0.0},

  external_clock(external_clock_),
  previous_external_clock(0.0),
  start_time(0.0),
  elapsed_time(0.0),
  end_time(0.0),
  dt(0.0),
  T_fast(0.0),
  T_slow(0.0),
  Freqw(0.0),
  decay_mult(0.0),
  p_factor_fast(0.0),
  p_factor_slow(0.0),
  ix_fast_next(0),
  ix_slow_next(0),
  fast_list(),
  slow_list(),
  fast_mag(),
  slow_mag(),
  sway_parallel(0.0),
  sway_normal(0.0),
  twist_angle(0.0),
  prev_sway_parallel(0.0),
  prev_sway_normal(0.0),
  prev_dp_up(0.0),
  prev_twist_sway_angle(),
  copy_wind_direction(0.0),
  sin_wind(0.0),
  cos_wind(1.0),
  small_angle_warning_sent(false)
{}

/*****************************************************************************
initialize
Purpose:(Initialize the instance)
*****************************************************************************/
void
TwistSway::initialize(
    double end_time_)
{
  if (!enabled) {
    return;
  }

  if (params.RocketHeight <= 0.0) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid parameters\n",
      "RocketHeight is set to an invalid value of ", params.RocketHeight, ".\n"
      "Aborting initialization of Twist-sway.\n");
    return;
  }

  // default twist-sway to terminate with provided value; this is typically
  // the pad-hold termination time.
  set_end_time( end_time_);

  //Random time between pertubations, uniformly distributed between
  //delta_T1_min and delta_T1_max.
  if (params.delta_T1_min < 0.0 || params.delta_T1_max < 0.0 ||
      params.delta_T2_min < 0.0 || params.delta_T2_max < 0.0) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid configuration on timing limits\n",
      "The specification of the limits for the decay constants T_fast and "
      "T_slow include a negative value.\n"
      "Cannot have a negative decay constant.\n"
      "Resetting all terms to their absolute values.\n");
    params.delta_T1_min = std::abs(params.delta_T1_min);
    params.delta_T1_max = std::abs(params.delta_T1_max);
    params.delta_T2_min = std::abs(params.delta_T2_min);
    params.delta_T2_max = std::abs(params.delta_T2_max);
  }
  std::uniform_real_distribution<double> dist1( params.delta_T1_min,
                                                params.delta_T1_max);
  T_fast = dist1(random_generator);
  p_factor_fast = std::exp (-T_fast / params.tau);

  std::uniform_real_distribution<double> dist2( params.delta_T2_min,
                                                params.delta_T2_max);
  T_slow = dist2(random_generator);
  p_factor_slow = std::exp (-T_slow / params.taul);

  if (MathUtils::is_near_equal( T_fast,0.0) ||
      MathUtils::is_near_equal( T_slow,0.0)) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Configuration_error\n",
      "One or both of the periods is close to zero.  Check configuration\n");
  }

  // high-frequency oscillation within the fast perturbation
  if (params.mode_freq_low < 0.0 || params.mode_freq_high < 0.0) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Invalid configuration on timing limits\n",
      "The specification of the limits for the oscillation frequency "
      "include a negative value.\n"
      "This could make the frequency negative.\nThe effect is minor -- "
      "only that the sign of the oscillatory addition will be negated.\n"
      "No action taken, but check config if this is unexpected.\n");
  }
  std::uniform_real_distribution<double> dist3( params.mode_freq_low,
                                                params.mode_freq_high);
  Freqw = dist3(random_generator);

  SubscriptionBase::initialize();
}


/*****************************************************************************
update
Purpose:
  Main executable to provide the state perturbations, expressed in the
  ENU-frame.
*****************************************************************************/
bool
TwistSway::update()
{
  if (!active ||
      MathUtils::is_near_equal( external_clock,
                                previous_external_clock)) {
    return false;
  }

  // RocketHeight != 0.0 is checked at initialization, but it is a public
  // variable so it is adjustable so it needs to be checked again before
  // dividing by it later in this algorithm.
  if (params.RocketHeight <= 0.0) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid parameters\n",
      "RocketHeight is set to an invalid value of ", params.RocketHeight, ".\n"
      "Aborting evaluation of Twist-sway.\n"
      "Disabling model\n");
    deactivate();
    return false;
  }

  if (external_clock > end_time) {
    deactivate();
    return true; // deltas have been zeroed.
  }

  // Figure out the time step and record current time for next step
  dt = external_clock - previous_external_clock;
  previous_external_clock = external_clock;
  elapsed_time = external_clock - start_time;

  // Check for approaching end of pad-alignment
  // FIXME Turner 03/2018
  //       This is not a great algorithm for tapering off the perturbations
  //       towards the end of the alignment phase.  The multiplicative factor
  //       is (1-e^(t^c))
  //       - The effect is unit-specific; the decay_const is not a
  //         time-constant, it is not clear what it represents physically
  //       - it is computationally inefficient; it requires calculation of a
  //         pow function just to determine whether it is needed.
  //       - It makes a pretty good approximation to a cliff-edge at small
  //         values of  decay_const, and the profile is extraordinarily
  //         sensitive to the selection of decay_const.
  //         (e.g. decay_const = 0.1 leaves ~50% of the amplitude at t = 0.01
  //               (presumably seconds, but the units are meaningless))
  //       - computation of decay_const is unnecessarily complicated, but here
  //         it is:
  //         - suppose you want some fraction, f, remaining in the last time
  //           interval, x.
  //         - decay_const = ln (-ln (1-f)) / ln (x)
  //         - f remaining at t = (-ln(1-f))^(1/decay_const)
  //       - profile is inflexible; all decay_const values produce 63%
  //         remaining at 1 time-unit from the termination of the model.
  //
  //       One of the advantages of this profile over a simple explonential is
  //       that for decay_const > 1, it has an inflection point, so approaches
  //       the target time more slowly than a simple exponential can.
  //       However, that can also be achieved with a cubic function.
  //
  //       A better strategy might be to replace this algorithm with either a
  //       simple cubic or an exponential tail-off; either would be easier to
  //       visualize, configure correctly, and compute.
  //       However, this was the original algorithm. There may have been a good
  //       reason for doing this (but it is undocumented if there was), so I'm
  //       leaving it here for now.
  double time_remaining = end_time - external_clock;
  double t_pow_k =  std::pow( time_remaining,
                         params.decay_const);
  if (t_pow_k > -params.limit_ln_epsilon) {
    decay_mult = 1.0;
  }
  else {
    decay_mult = 1 - std::exp( -t_pow_k);
  }

  check_for_active_perturbations();
  accumulate_perturbations();
  compute_twist_sway_enu();
  return true;
}

/*****************************************************************************
update_ecef
Purpose:
  Main executable to provide the state perturbations, expressed in the ECEF
  frame.
Notes / Assumptions:
 - This method requires advance population of:
    - Q_ecef_to_enu
 - This method assumes prior execution of update().  update() computes the
   perturbations and expressed them in the ENU frame;  this method converts
   those values into the ECEF frame.
*****************************************************************************/
void
TwistSway::update_ecef()
{
  if (!active) {
    return;
  }

  Q_enu_to_ecef.left_quat_transform( dp_enu,
                                     dp_ecef);
  Q_enu_to_ecef.left_quat_transform( dv_enu,
                                     dv_ecef);
  // Q_ECEF_to_TS = Q_ENU_to_TS  * (Q_ENU_to_ECEF)*
  Q_enu_to_ts.multiply_conjugate(   Q_enu_to_ecef,
                                    Q_ecef_to_ts);
  Q_enu_to_ecef.left_quat_transform(  w_ts_wrt_enu_in_enu,
                                      w_ts_wrt_enu_in_ecef);
}


/*****************************************************************************
new_*_perturbation
Purpose:(Adds a new perturbation to the high-frequency or low-frequency list)
*****************************************************************************/
void
TwistSway::new_fast_perturbation()
{
  TwistSwayMagnitudes new_perturb;
  std::uniform_real_distribution<double> dist( 0,1);
  new_perturb.parallel = dist(random_generator) * params.Parallel_Motion_Fast;
  new_perturb.normal   = dist(random_generator) * params.Normal_Motion_Fast;
  new_perturb.twist    = dist(random_generator) * params.Twist_Mag;
  fast_list.push_front( new_perturb);
}
/****************************************************************************/
void
TwistSway::new_slow_perturbation()
{
  TwistSwayMagnitudes new_perturb;
  std::uniform_real_distribution<double> dist( 0,1);
  new_perturb.parallel = dist(random_generator) * params.Parallel_Motion_Slow;
  new_perturb.normal   = dist(random_generator) * params.Normal_Motion_Slow;
  new_perturb.twist    = 0.0;
  slow_list.push_front( new_perturb);
}

/*****************************************************************************
check_for_active_perturbations
Purpose:(checks current time for status of perturbations; adds new ones and
         removes decayed ones as necessary.)
*****************************************************************************/
void
TwistSway::check_for_active_perturbations()
{
  // if current time is more than period past the last perturbation added, add
  // a new one.
  if (!MathUtils::is_near_equal( T_fast,0.0)) {
    while ( elapsed_time >= T_fast * ix_fast_next) {
      new_fast_perturbation();
      ix_fast_next++;
    }
  }
  if (!MathUtils::is_near_equal( T_slow,0.0)) {
    while ( elapsed_time > T_slow * ix_slow_next) {
      new_slow_perturbation();
      ix_slow_next++;
    }
  }

  // if the front (oldest) perturbation has decayed away, remove it.
  // Perturbation will decay away when
  //      exp(-(t - t_0)/tau) < epsilon
  // Thus, there is a "removal-time" at which a perturbation can be removed
  //      t_remove = t_0 - tau * ln(epsilon)
  // where t_0 is the time at which the perturbation started to decay, i.e.:
  //   - the start-time of the perturbation following "this" for mode
  //     GrowHoldDecay (which is when "this" starts to decay).
  //   - the start-time of "this" perturbation for ImpulseDecay and GrowDecay

  // Only need to check for removal when there are perturbations currently
  // being processed
  if (!fast_list.empty()) {
    double t_remove = T_fast * (ix_fast_next - fast_list.size()) -
                         params.tau * params.limit_ln_epsilon;
    if (perturb_algorithm == GrowHoldDecay) {
      t_remove += T_fast;
    }
    while ( elapsed_time > t_remove) {
      fast_list.pop_back();
      t_remove += T_fast;
    }
  }

  // Repeat for the slow perturbations.
  // Note - this is almost code duplication except that the time-constant
  // changed from tau to taul.
  if (!slow_list.empty()) {
    double t_remove = T_slow * (ix_slow_next - slow_list.size()) -
                         params.taul * params.limit_ln_epsilon;
    if (perturb_algorithm == GrowHoldDecay) {
      t_remove += T_slow;
    }
    while ( elapsed_time > t_remove) {
      slow_list.pop_back();
      t_remove += T_slow;
    }
  }
}

/*****************************************************************************
accumulate_perturbations
Purpose:(Collects and sums the individual perturbations to give an overall
         state.)
*****************************************************************************/
void
TwistSway::accumulate_perturbations()
{
  if (!fast_list.empty()) {
    // In document, this is the value eta
    double p_factor = std::exp(-(elapsed_time - ((ix_fast_next - 1) * T_fast)) /
                                params.tau);
    switch (perturb_algorithm) {
    case GrowHoldDecay :
    {
      fast_mag.parallel = fast_list.front().parallel * (1-p_factor);
      fast_mag.normal   = fast_list.front().normal   * (1-p_factor);
      fast_mag.twist    = fast_list.front().twist    * (1-p_factor);
      p_factor *= (1 - p_factor_fast);
      // use an iterator rather than simple whole-list because starting with
      // element #2.
      std::list< TwistSwayMagnitudes >::iterator iter;
      for ( iter = ++fast_list.begin(); // element 2
            iter != fast_list.end();
            ++iter) {
        fast_mag.parallel += (*iter).parallel * p_factor;
        fast_mag.normal   += (*iter).normal   * p_factor;
        fast_mag.twist    += (*iter).twist    * p_factor;
        p_factor *=  p_factor_fast;
      }
      break;
    }
    case ImpulseDecay:
      fast_mag.parallel = 0.0;
      fast_mag.normal = 0.0;
      fast_mag.twist = 0.0;
      for( const TwistSwayMagnitudes & iter : fast_list) {
        fast_mag.parallel += iter.parallel * p_factor;
        fast_mag.normal   += iter.normal   * p_factor;
        fast_mag.twist    += iter.twist    * p_factor;
        p_factor *= p_factor_fast;
      }
      break;
    case GrowDecay:
      fast_mag.parallel = 0.0;
      fast_mag.normal = 0.0;
      fast_mag.twist = 0.0;
      for( const TwistSwayMagnitudes & iter : fast_list) {
        fast_mag.parallel += 4 * iter.parallel * p_factor * (1-p_factor);
        fast_mag.normal   += 4 * iter.normal   * p_factor * (1-p_factor);
        fast_mag.twist    += 4 * iter.twist    * p_factor * (1-p_factor);
        p_factor *= p_factor_fast;
      }
      break;
    }
  }

  // Repeat with the slow-perturbation.
  if (!slow_list.empty()) {
    double p_factor = std::exp(-(elapsed_time - ((ix_slow_next - 1) * T_slow)) /
                           params.taul);
    switch (perturb_algorithm) {
    case GrowHoldDecay :
    {
      slow_mag.parallel = slow_list.front().parallel * (1-p_factor);
      slow_mag.normal   = slow_list.front().normal   * (1-p_factor);
      p_factor *= (1 - p_factor_slow);
      // use an iterator rather than simple whole-list because starting with
      // element #2.
      std::list< TwistSwayMagnitudes >::iterator iter;
      for ( iter = ++slow_list.begin(); // element 2
            iter != slow_list.end();
            ++iter) {
        slow_mag.parallel += (*iter).parallel * p_factor;
        slow_mag.normal   += (*iter).normal   * p_factor;
        p_factor *=  p_factor_slow;
      }
      break;
    }
    case ImpulseDecay:
      slow_mag.parallel = 0.0;
      slow_mag.normal = 0.0;
      for( const TwistSwayMagnitudes & iter : slow_list) {
        slow_mag.parallel += iter.parallel * p_factor;
        slow_mag.normal   += iter.normal   * p_factor;
        p_factor *= p_factor_slow;
      }
      break;
    case GrowDecay:
      slow_mag.parallel = 0.0;
      slow_mag.normal = 0.0;
      for( const TwistSwayMagnitudes & iter : slow_list) {
        slow_mag.parallel += 4 * iter.parallel * p_factor * (1-p_factor);
        slow_mag.normal   += 4 * iter.normal   * p_factor * (1-p_factor);
        p_factor *= p_factor_slow;
      }
      break;
    }
  }
}


/*****************************************************************************
compute_sway_enu
Purpose:(Computes the sway effects in an East-North-Up reference frame.)
*****************************************************************************/
void
TwistSway::compute_twist_sway_enu()
{
  // Add the sine oscillation to the fast perturbation, and add the modified
  // fast perturbation and the slow perturbation to get the total
  // perturbation. Then further scale by multiplying by decay_mult to scale
  // back the perturbations if we are near the end of the perturbation
  // process.
  double sin_func = std::sin(2 * M_PI * Freqw * elapsed_time);

  sway_parallel = (fast_mag.parallel* sin_func + slow_mag.parallel) *decay_mult;
  sway_normal =   (fast_mag.normal* sin_func + slow_mag.normal) * decay_mult;
  twist_angle =   fast_mag.twist * sin_func * decay_mult;

 //Calculate the wind induced sway motion in North, East, Up frame
  // First, recompute the wind direction if it has been reset.
  if (!MathUtils::is_near_equal(copy_wind_direction, params.wind_direction)) {
    copy_wind_direction = params.wind_direction;
    sin_wind = std::sin(params.wind_direction);
    cos_wind = std::cos(params.wind_direction);
  }

  // The body is going to move and rotate to a new position and attitude,
  // generating a new reference frame.  We need the state of this frame
  // (position, velocity, attitude, attitude rate) relative to the ENU frame,
  // stationary and located at the body base.
  // The new coordinates are (e, n, u), with magnitude H
  // NOTE - using single letter variable names for generating the
  //        transformation matrix.
  double H = params.RocketHeight;
  double e = - cos_wind * sway_normal + sin_wind * sway_parallel;
  double n =   sin_wind * sway_normal + cos_wind * sway_parallel;
  double h2 = e*e + n*n; // square of horizontal
  double u2 = H*H - h2;

  // Magic number (0.2). This value is largely arbitrary, but it corresponds to an
  // angle of about 11 degrees at which point the small angle approximations really
  // start breaking down.
  // If the stack has swayed by more than that, something is wrong.
  if (h2 > 0.2 * H*H) {
    if (!small_angle_warning_sent) {
      CMLMessage::warn(
        __FILE__,__LINE__,"Excessive twist-sway detected\n",
        "The linear twist-sway settings have been configured such that the\n"
        "twist-sway motion exceeds the small angle limitation on which the\n"
        "algorithm is based.\n"
        "This violates the assumptions of the model.\n"
        "Results are unreliable\n");
      small_angle_warning_sent = true;
    }

    // At this point the algorithm needs terminal protection because we are about
    // to take the square-root of u2. u2 cannot be negative; even if the stack
    // was laid down horizontally, the conceptual u2 would then be 0.0.
    if (u2 <= 0.0) {
      CMLMessage::error(
        __FILE__,__LINE__,"Excessive twist-sway detected\n",
        "The linear twist-sway settings have been configured such that the\n"
        "linear twist sway motion now exceeds the reference length.\n"
        "This is physically invalid, even if the stack had fallen over.\n"
        "Disabling model\n");
      deactivate();
      return;
    }
  }
  double u =  std::sqrt( u2);

  // East and North
  dp_enu[0] = e;
  dp_enu[1] = n;
  // Up found from Pythagoras
  dp_enu[2] = u - H;

 //Calculate the vertical component of motion

  // The sway rates are simply generated from the backward difference
  // of the positions
  // Note: dt != 0 verification made at top of update method
  double rate_up  = (dp_enu[2] - prev_dp_up) / dt;
  double sway_rate_parallel = (sway_parallel - prev_sway_parallel) / dt;
  double sway_rate_normal = (sway_normal - prev_sway_normal) / dt;

  // Calculate the wind induced sway rate in North, East, Up frame
  dv_enu[0] = -cos_wind * sway_rate_normal + sin_wind * sway_rate_parallel;
  dv_enu[1] =  sin_wind * sway_rate_normal + cos_wind * sway_rate_parallel;
  dv_enu[2] =  rate_up;

  // apply the sway
  jeod::Quaternion Q_enu_to_sway; // initializes to identity
  // Check for valid config before assigning values to quaternion:
  // Otherwise, keep the default identity quaternion.
  // Dividing by H(H+u)
  if ( !MathUtils::is_near_equal( H*(H+u), 0.0)) {
    Q_enu_to_sway.scalar = H + u;
    Q_enu_to_sway.vector[0] = n;
    Q_enu_to_sway.vector[1] = -e;
    Q_enu_to_sway.vector[2] = 0;
    Q_enu_to_sway.scale( 1 / std::sqrt(2*H*(H+u)));
  }

  // apply the twist:
  double q_vec[3] =  {0, 0, -std::sin(twist_angle/2)};
  jeod::Quaternion Q_sway_to_ts( std::cos(twist_angle/2),
                           q_vec);

  Q_sway_to_ts.multiply( Q_enu_to_sway,
                         Q_enu_to_ts);


  // Compute angles using small angle approximation
  // Note - This is a mathematical array for convenience, it is not a vector in
  //        the physical sense. Indices [0] and [1] are in ENU frame,
  //        index [2] is in Sway frame */

  // Note: H > 0 and dt != 0 verification made at top of update method
  double twist_sway_angle[3] = { e * twist_angle - n,
                                 n * twist_angle + e,
                                 u * twist_angle};
  jeod::Vector3::scale( 1.0 / H,
                  twist_sway_angle);
  // Calculate the body rates as an Euler-difference of the sway-angles
  jeod::Vector3::diff( twist_sway_angle,
                 prev_twist_sway_angle,
                 w_ts_wrt_enu_in_enu);
  jeod::Vector3::scale( 1.0 / dt,
                  w_ts_wrt_enu_in_enu);

  // Overwrite the record for computation next time.
  prev_dp_up = dp_enu[2];
  prev_sway_parallel = sway_parallel;
  prev_sway_normal = sway_normal;
  jeod::Vector3::copy( twist_sway_angle, prev_twist_sway_angle);
}


/*****************************************************************************
set_end_time
Purpose:(Called when trying to set the pad-hold time; checks against the
         pad-hold time for which this model was initialized and arrays
         allocated.)
*****************************************************************************/
void
TwistSway::set_end_time(double new_time)
{
  if (new_time < external_clock)  {
    CMLMessage::error(
      __FILE__,__LINE__,"Reconfiguration error.\n",
      "Attempt to define the end of the hold-time to ", new_time, ", which is in the past\n"
      "(current time is ", external_clock, ").\n"
      "Twist-sway will be shut-off immediately, but that may result in\n"
      "a step-function in the state as the current displacement is\n"
      "instantaneously zeroed.");
    deactivate();
  }
  end_time = new_time;
}

/*****************************************************************************
activate
Purpose:(Activates the model)
*****************************************************************************/
void
TwistSway::activate()
{
  start_time = external_clock;
  SubscriptionBase::activate();
}
/*****************************************************************************
deactivate
Purpose:(Turns the model off)
*****************************************************************************/
void
TwistSway::deactivate()
{
  jeod::Vector3::initialize(dp_enu);
  jeod::Vector3::initialize(dv_enu);
  Q_enu_to_ts.make_identity();
  jeod::Vector3::initialize(w_ts_wrt_enu_in_enu);
  jeod::Vector3::initialize(dp_ecef);
  jeod::Vector3::initialize(dv_ecef);
  Q_ecef_to_ts.make_identity();
  jeod::Vector3::initialize(w_ts_wrt_enu_in_ecef);
  sway_parallel = 0.0;
  sway_normal = 0.0;
  twist_angle = 0.0;
  prev_sway_parallel = 0.0;
  prev_sway_normal = 0.0;
  prev_dp_up = 0.0;
  jeod::Vector3::initialize(prev_twist_sway_angle);
  fast_list.clear();
  slow_list.clear();
  SubscriptionBase::deactivate();
}
