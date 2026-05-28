/*********************************************************************************
PURPOSE:
    (Provides a way to remove the discontinuities in the wind model when
     atmosphere_exec is called at a slower rate than dynamics.)

PROGRAMMERS:
    (
    ((Gary Turner) (OSR) (Oct 2014) (Antares))
    ((Bingquan Wang) (OSR) (Apr 2017) (Antares) (Fixed the compilation warning of
                                      float-point number equality compsarison))
    ((Brent Caughron) (OSR) (July 2017) (Antares) (IV&V model code cleanup))    )

**********************************************************************************/
#define _USE_MATH_DEFINES //M_PI
#include <cmath> //M_PI, atan2
#include <limits> //numeric_limits

#include "jeod/models/utils/math/include/vector3.hh"

#include "../include/first_order_hold.hh"


/*******************************************************************************
Method:
Purpose: Constructor
*******************************************************************************/
FirstOrderHold::FirstOrderHold(
     const double & dyn_time_in,
     const ExtendedPlanetaryDerivedState & planet_state_in,
     AtmosExecOutput & atmos_output_in)
  :
  FO_hold(false),
  FO_init(false),
  reference_time(0.0),
  previous_hold_time(0.0),
  slope_valid_time(0.0),
  reference_wind(),
  slope_reference_wind(),
  slope(),
  dyn_time(dyn_time_in),
  planet_state(planet_state_in),
  out(atmos_output_in)
{ }

/*****************************************************************************
activate
Purpose:(Activates the model as needed.)
*****************************************************************************/
void
FirstOrderHold::activate()
{
  reference_time = dyn_time;
  jeod::Vector3::copy( out.wind_velocity_td,
                 reference_wind);
  FO_init = true;
  return;
}
/*****************************************************************************
deactivate
Purpose:(Turns the model off when it is no longer needed.)
*****************************************************************************/
void
FirstOrderHold::deactivate()
{
  FO_init = false;
}

/*****************************************************************************
update
Purpose: (Provide a method to remove discontinuities in wind model when
          atmosphere_exec is called at a slower rate than the dynamics)
Assumptions: (This method should be called AFTER the call to update
              AtmosphereExec.  Otherwise, any change made by AtmosphereExec
              will be raw and unsmoothed, then get smoothed at the next cycle.)
Limitations: (Assumes time advances forwards.  Will break in reverse.)
PROGRAMMERS:
    (((Mark Kane) (NASA-EG4) (02/09) (ORION) (initial version))
     ((Gary Turner) (OSR) (10/14) (Antares) (Adaptation for C++)))
*******************************************************************************/
void
FirstOrderHold::update()
{
  // Return if not active
  if(!FO_hold || !FO_init) {
    return;
  }
   
  // If there has been an atmosphere update since the last time here:
  if(std::fabs(out.atmos_new_time-reference_time) >= 
               std::numeric_limits<double>::min()) {
    // Time since last call to this method:
    double this_to_now_delta_t    = dyn_time -  previous_hold_time;
    // Time between atmos calls:
    double atmos_to_atmos_delta_t = out.atmos_new_time - reference_time;
    // In both of the following cases, record the latest time from the
    //  atmosphere to go with the reference_wind and record the wind from the
    //  previously known atmosphere as the start of the slope.
    reference_time = out.atmos_new_time;
    jeod::Vector3::copy( reference_wind,
                   slope_reference_wind);

    // Now branch:
    // If more or equal time elapsed between calls to this method and calls to
    // the atmosphere update, then Atmosphere being called faster or at the
    // same rate as the wind-hold.  Copy the atmos-data into the reference set
    // and do not adjust the atmos-out.
    // Record this time as the most recent visit.
    // This method has no effect, so get out of here.
    // NOTE - this test could fail if the dynamics and atmos updates are
    //        asynchronous.  It only tests the most recent atmos update against
    //        the most recent atmos update at the previous call to this method.
    if ( this_to_now_delta_t >= atmos_to_atmos_delta_t)  {
      jeod::Vector3::copy( out.wind_velocity_td,
                     reference_wind);
      jeod::Vector3::initialize( slope);
      previous_hold_time = dyn_time;
      return;
    }

    // Otherwise, wind-hold being called faster, and need to interpolate between
    // atmos-exec output data.  But it is not possible to interpolate between a
    // known point and a future point, so instead the simulation gets retarded
    // one cycle (atmos-exec - to - atmos-exec cycle), and the atmospheric winds
    // are computed as an interpolation between the previous two atmos-exec 
    // outputs.
    // else:
    for (unsigned int ii=0; ii < 3 ; ii++) {
      // slope goes from there to the most recent reference point.      
      slope[ii] = (out.wind_velocity_td[ii] - reference_wind[ii]) /
                   atmos_to_atmos_delta_t;
    }

    // Also record the timestep over which this slope is valid.
    slope_valid_time = atmos_to_atmos_delta_t;
 
    // temporarily store off the atmosphere output so that it can be swapped
    double new_wind[3];
    jeod::Vector3::copy(out.wind_velocity_td,
                  new_wind);
    // Now push the reference wind onto the current wind (simulation will lag
    // behind by one atmosphere-update cycle)
    jeod::Vector3::copy(reference_wind,
                  out.wind_velocity_td);
    // And push the latest output from the atmosphere into the reference slot
    // for next time around.
    jeod::Vector3::copy(new_wind,
                  reference_wind);
  }
  // Else, No atmosphere update since last time here.
  else {
    // Time since last recorded atmos update:
    double atmos_to_now_delta_t  = dyn_time - reference_time;
    // If the slope is still valid, use it to interpolate between the known
    // points:
    if ( atmos_to_now_delta_t < slope_valid_time) {
      for (unsigned int ii=0; ii < 3 ; ii++) {
        out.wind_velocity_td[ii] = slope_reference_wind[ii] + 
                           slope[ii] * atmos_to_now_delta_t;
      }
    }

    else if ( std::fabs(slope_valid_time) > 0.0) {
      jeod::Vector3::copy(reference_wind,
                    out.wind_velocity_td);
      slope_valid_time = 0.0;
    }
  }
  
  // Record this time as the most recent visit.
  previous_hold_time = dyn_time;
  
  jeod::Vector3::transform_transpose( planet_state.topodetic.T_inrtl_to_this,
                                out.wind_velocity_td,
                                out.wind_velocity_eci);
  jeod::Vector3::transform( planet_state.topocentric.T_inrtl_to_this,
                      out.wind_velocity_eci,
                      out.wind_velocity_tc);
  out.wind_vmag = jeod::Vector3::vmag( out.wind_velocity_td );

  // Compute wind angle blowing to (taken from gram_wrapper_E10.f90:192)
  out.wind_angle_blowing_to = std::atan2( out.wind_velocity_tc[1],
                                          out.wind_velocity_tc[0]);

  if ( out.wind_angle_blowing_to < 0.0 ) {
    out.wind_angle_blowing_to += 2.0*M_PI;
  }
  // Compute wind angle blowing from (taken from gram_wrapper_E10.f90:198)
  if (out.wind_angle_blowing_to >= M_PI) {
    out.wind_angle_blowing_from = out.wind_angle_blowing_to - M_PI;
  } else {
    out.wind_angle_blowing_from = out.wind_angle_blowing_to + M_PI;
  }
  return;
}
