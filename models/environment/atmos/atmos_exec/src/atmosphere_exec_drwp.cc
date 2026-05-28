/******************************** TRICK HEADER **********************************
PURPOSE:
   (Interface for the DRWP atmosphere.)

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (May 2016) (ANTARES)
       (New architecture to isolate the atmospheres from the master executive))
    ((Bingquan Wang) (OSR) (April 2017) (ANTARES)
       (Disabled the compilation warning of float-point number comparison))
   )
********************************************************************************/

#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/atmosphere_exec_drwp.hh"


/*****************************************************************************
Purpose:(Construct the class)
*****************************************************************************/
AtmosphereExec_Drwp::AtmosphereExec_Drwp(
           LookupAtmosWinds              & DRWP_atmos_in,
           ExtendedPlanetaryDerivedState & planet_state_in,
           AtmosExecOutput               & master_output)
  :
  AtmosphereExec_AtmosWindsBase(master_output, planet_state_in),
  DRWP_atmos(DRWP_atmos_in),
  initialized(false),
  altitude_at_prev_update(0.0)
{
  name = "DRWP";
}

/*****************************************************************************
activate
Purpose:(Activates the model, creates necessary subscriptions)
*****************************************************************************/
void
AtmosphereExec_Drwp::activate()
{
  if (active) return;

  subscribe_to_topocentric_altitude();
  AtmosphereExec_AtmosBase::activate();
  DRWP_atmos.subscribe();
}

/*****************************************************************************
deactivate
Purpose:(Removes subscriptions)
*****************************************************************************/
void
AtmosphereExec_Drwp::deactivate()
{
  if (active) {
    planet_state.unsubscribe_topocentric_altitude();
    AtmosphereExec_AtmosBase::deactivate();
    DRWP_atmos.unsubscribe();
  }
}

/*****************************************************************************
initialize
Purpose:(multi-purpose, used to initialize the atmosphere and the winds)
*****************************************************************************/
bool
AtmosphereExec_Drwp::initialize()
{
  if (!initialized) {
    DRWP_atmos.initialize();
    initialized = DRWP_atmos.is_initialized();
  }
  return true;
}

/*****************************************************************************
Purpose:(main update loop for atmos, interface to Drwp update, collects outputs)
*****************************************************************************/
void
AtmosphereExec_Drwp::update_atmos()
{
  // Quick check that update is necessary:
  if (!MathUtils::is_near_equal(altitude_at_prev_update,planet_state.topocentric_altitude)) {
    DRWP_atmos.update(planet_state.topocentric_altitude);
    altitude_at_prev_update = planet_state.topocentric_altitude;
  }

  out.density           = DRWP_atmos.rho;
  out.temperature       = DRWP_atmos.T;
  out.pressure          = DRWP_atmos.P;
  out.speed_of_sound    = DRWP_atmos.SOS;
  out.dynamic_viscosity = 0.0;
}

/*****************************************************************************
Purpose:(main update loop for winds, interface to Drwp update, collects outputs)
*****************************************************************************/
void
AtmosphereExec_Drwp::update_winds()
{
  // Quick check that update is necessary:
  if (!MathUtils::is_near_equal(altitude_at_prev_update, planet_state.topocentric_altitude)) {
    DRWP_atmos.update( planet_state.topocentric_altitude);
    altitude_at_prev_update = planet_state.topocentric_altitude;
  }

  out.wind_angle_blowing_from = DRWP_atmos.wind_angle_blowing_from;
  out.wind_angle_blowing_to   = DRWP_atmos.wind_angle_blowing_to;
  out.wind_vmag               = DRWP_atmos.wind_vmag;

  // DRWP_atmos uses float, so do not try to utilize jeod::Vector3:: for this,
  // just go with direct element-by-element assignment.
  out.wind_velocity_tc[0] = DRWP_atmos.wind_velocity_tc[0];
  out.wind_velocity_tc[1] = DRWP_atmos.wind_velocity_tc[1];
  out.wind_velocity_tc[2] = DRWP_atmos.wind_velocity_tc[2];

  winds_from_topocentric();
}
