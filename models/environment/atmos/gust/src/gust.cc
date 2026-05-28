/*********************************************************************************
PURPOSE:
    (Simple 1 - cosine gust model.)

PROGRAMMERS:
    (((Jon Berndt) (ESCG/Jacobs) (02/08) (ANTARES) (initial version))
     ((Gary Turner) (OSR) (2016) (Antares) (C++))
     ((Brent Caughron) (OSR) (Sept 2017) (Antares) (IV&V code review))
    )
**********************************************************************************/
#define _USE_MATH_DEFINES // M_PI
#include <cmath> // std::cos, M_PI

#include "jeod/models/utils/math/include/vector3.hh" // Vector3

#include "cml/models/utilities/math_utils/include/math_utils.hh" // MathUtils::*

#include "../include/gust.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
GustModel::GustModel(const double & time_in)
  :
  time(time_in),

  phase_angle(),
  dir_unit_vector(),
  elapsed_time(),
  start_time(),
  active(false),
  period_message_sent(false),

  gust_frame(TD_FRAME),
  direction{0.0,0.0,0.0},
  maximum_magnitude(0.0),
  magnitude(0.0),
  gust_vector{0.0,0.0,0.0},
  period(0.0),
  start(false)
{}

/*****************************************************************************
activate
Purpose:(Function to turn the gusting model on)
*****************************************************************************/
void
GustModel::activate()
{
  // Protection against accidentally restarting the start_time
  if (active) return;
  if (period < 0.0 ) {
    if (!period_message_sent) {
      CMLMessage::error ( __FILE__,__LINE__,
        "The period has been set to an invalid value < 0.0\n"
        "Change period to a positive value.\n"
        "Activation failed.\n");
      period_message_sent = true;
    }
    return;
  }
  if (!MathUtils::has_changed_from( period, 0.0)) {
    if (!period_message_sent) {
      CMLMessage::error ( __FILE__,__LINE__,
        "The gust period has not been set, or set to an invalid value = 0.0\n"
        "Change period to a positive value.\n"
        "Activation failed.\n");
      period_message_sent = true;
    }
    return;
  }
  period_message_sent = false;

  // Record the start time, activate the model and indicate that the gust
  // shall stop after this cycle.
  start_time = time;
  active = true;
  start = false;
}

/*****************************************************************************
update
Purpose:(Main executive)
*****************************************************************************/
void
GustModel::update()
{

  if (!active){
    if (!start) return;
    activate();
    if (!active) return;
  }

  elapsed_time = time - start_time;
  if (elapsed_time > period) {
    if (!start) {
      deactivate();
      return;
    }
    // else start a new gust.
    start_time = time;
    elapsed_time = 0;
    start = false;
  }

  phase_angle = (elapsed_time / period) * 2 * M_PI;
  magnitude = (maximum_magnitude/2.0) * (1.0 - std::cos(phase_angle));

  /* Allow the direction to be modified within a gust, so re-normalize the
     gust direction each cycle.*/
  jeod::Vector3::normalize(direction, dir_unit_vector);
  jeod::Vector3::scale(dir_unit_vector, magnitude, gust_vector);
}
/*******************************************************************************
 deactivate
Purpose:(Gives a way to turn off the model mid gust if desired)
*******************************************************************************/
void
GustModel::deactivate()
{
  active = false;
  magnitude = 0.0;
  MathUtils::zero_vector(gust_vector);
  //jeod::Vector3::initialize(gust_vector);
}
