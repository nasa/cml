/*******************************TRICK HEADER******************************
PURPOSE: (The RcsJetGroup provides a convenient mechanism for grouping
  like-Rcs-jets together to give them identical behavioral configurations.
  In the C-based model on which this object is based, the RCSGENERIC model
  had several instances of RCS_MODEL that needed instantiating; this object
  represents a very similar concept to RCS_MODEL.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (April 2017) (Antares)
       (initial object-oriented implementation))
  )
**********************************************************************/

#include <cmath>  // abs
#include "../include/rcs_group.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
RcsJetGroup::RcsJetGroup(
  const unsigned int & num_prop_components_)
  :
  consumption_epsilon (1.0e-12),
  num_prop_components (num_prop_components_),
  blow_down (false),
  propc_use_isp(false),
  signal_delay_time(0.0),
  on_dead_time(0.0),
  off_dead_time(0.0),
  build_up_time(0.0),
  trail_off_time(0.0),
  min_on_time(0.0),
  min_off_time(0.0),
  mixture_ratio (0.0),
  isp_prop_comp_ratio(num_prop_components, 0.0),
  bd_force_coef(),
  bd_isp_coef(),
  bd_pressure_limit(0.0),
  buffer_flag(false),
  buffer_on_size (0),
  buffer_off_size(0),
  delay_time_on(0.0),
  delay_time_off(0.0)
{}



/*****************************************************************************
intialize
Purpose:()
*****************************************************************************/
void
RcsJetGroup::initialize(
    double time_step)
{
  /************************************************/
  /* Set up command buffers and initialize delays */
  /************************************************/
  /* total on delay is sum of signal delay and valve reaction time (dead_time) */
  double total_on_delay = signal_delay_time + on_dead_time;
  /* total off delay is sum of signal delay and valve reaction time (dead_time) */
  double total_off_delay = signal_delay_time + off_dead_time;

  // Check to see if a buffer is needed:
  // if on or off delays are equal or greater than one time_step, then
  // commands must be buffered
  buffer_flag = ((total_on_delay >=  time_step) ||
                 (total_off_delay >= time_step));

  // buffer size is the number of full time-steps necessary before a command
  // will be seen
  buffer_on_size  = static_cast<int>(total_on_delay  / time_step);
  buffer_off_size = static_cast<int>(total_off_delay / time_step);

  /* delay time = remainder of last time_step before jet is turned on or off */
  delay_time_on = total_on_delay - buffer_on_size * time_step;
  delay_time_off = total_off_delay - buffer_off_size * time_step;


  // Check the propulsion consumption ratios for the case of isp-determined
  // usage.
  if (propc_use_isp) {
    if (num_prop_components > 1){ /* multi-propellant case */
      double sum_comp_ratio_ = 0.0;
      // Add up the values of isp_prop_comp_ratio.  It should come to 1.0
      for (std::vector<double>::iterator it = isp_prop_comp_ratio.begin();
           it != isp_prop_comp_ratio.end();
           ++it) {
        sum_comp_ratio_ += (*it);
      }
      // Protection against missing isp_prop_comp_ratio setting
      if( MathUtils::is_near_equal( sum_comp_ratio_, 0.0)){
        CMLMessage::fail(
        __FILE__,__LINE__,"Incomplete configuration.\n",
        "Each model's 'isp_prop_comp_ratio' vector must be populated when\n"
        "using ISP for propellant consumption calculations.\n");
      }

      if (!MathUtils::is_near_equal( sum_comp_ratio_, 1.0) ) {
        CMLMessage::warn(
        __FILE__,__LINE__,"Incorrect Configuration.\n",
        "When using isp for computing prop comsumption, the sum of the ratios\n"
        "of the propulsion components (isp_prop_comp_ratio) should equal 1.0\n"
        "It instead has value ", sum_comp_ratio_, "\n"
        "Normalizing the values to prevent incorrect propellant usage.\n");

        for (std::vector<double>::iterator it=isp_prop_comp_ratio.begin();
             it != isp_prop_comp_ratio.end();
             ++it) {
           (*it) /= sum_comp_ratio_;
        }
      }
      // Otherwise, the isp_prop_comp_ratio values are appropriately valued.
    }
    else {
      isp_prop_comp_ratio.at(0) = 1.0;
    }
  }
}

/*****************************************************************************
set_blow_down
Purpose:( if blow-down has been set, checks that there is only 1 component.)
*****************************************************************************/
void
RcsJetGroup::set_blow_down(
    bool blow_down_)
{

  if (blow_down_ && num_prop_components != 1) {
    CMLMessage::error(
    __FILE__,__LINE__,"Invalid setting.\n",
    "Cannot use blowdown on other than a mono-propellant\n"
    "There are ", num_prop_components, " propulsion components\n");
    return;
  }
  blow_down = blow_down_;
}