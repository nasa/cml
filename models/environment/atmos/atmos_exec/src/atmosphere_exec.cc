/*********************************************************************************
PURPOSE:
    (Atmosphere executive to provide the option to select between various
     atmosphere and wind models.)

PROGRAMMERS:
    (
     ((Brian Hoelscher) (NASA-EG4) (06/06) (ARES) (initial version))
     ((Pat Galvin) (ESCG) (07/06) (ARES) (wind vector changes - rdlaa09011))
     ((Pat Galvin) (ESCG) (8/06) (ARES) (add Patrick AFB winds database - rdlaa08980))
     ((Pat Galvin) (ESCG) (8/06) (ARES) (add Patrick AFB atmos database))
     ((Pat Galvin) (ESCG) (12/06) (ANTARES) (add special winds database - rdlaa09288))
     ((Pat Galvin) (ESCG) (12/06) (ANTARES) (add special atmos database - rdlaa09456))
     ((Pat Galvin) (ESCG) (1/07) (ANTARES) (add KSC winds - rdlaa09505))
     ((Brian Hoelscher) (NASA) (06/07) (ANTARES) (GRAM speed of sound limit - rdlaa09843))
     ((Pat Galvin) (ESCG) (07/07) (rdlaa09781) (add wind comp input option ))
     ((Jeremy Rea) (NASA) (01/08) (ANTARES) (RDLaa10259: Add dynamic viscosity computation))
     ((Brian Bihari) (ESCG) (12/07) (ANTARES) (Implement GRAM 2007 - rdlaa09780))
     ((Michael Rosburg) (ESCG) (05/08) (ANTARES) (RDLaa10236))
     ((Pat Galvin) (ESCG) (10/08) (RDLaa10722) (Remove GRAM atmos reqt for GRAM winds))
     ((Pat Galvin) (ESCG) (12/08) (AGDL00001242) (Use topocentric inputs to GRAM))
     ((Geoffrey Bixby) (USA) (04/09) (AGDL00002423) (Added comments indicating
        exactly which altitude was needed by GRAM and eliminated mean-sea-level
        calculations for GRAM atmosphere calls))
     ((Ryan Stillwater) (NASA-DFRC) (5/09) (AGDL00002617) (Implement ATB Atmosphere and Winds))
     ((Jose Varela) (ESCG) (06/10) (AGDL200000236) (Correct Special Wind Model Vertical Component for Topodetic Vector Input Option))
     ((Brian Bihari) (ESCG) (08/10) (ANTARES) (Implement GRAM 2010 - AGDL200000284))
     ((Brian Bihari) (ESCG) (08/11) (ANTARES) (Implement GRAM Month  AGDL200000656))
     ((Gary Turner) (OSR) (2014) (ANTARES) (Upgrade to C++ for Trick13))
     ((Gary Turner) (OSR) (2016) (ANTARES)
              (Refactor to make the individual atmos-execs add-ons and
               provide a lightweight interface for external models to see))
     ((Bingquan Wang) (OSR) (2017) (ANTARES) (Fixed the compilation warning of
               float-point number equality comparison))
     ((Susan Stachowiak) (NASA-JSC) (05/2017) (ANTARES) (Add ability to switch to DRWP winds mid-run))
     ((Dan Jordan) (JETS) (11/2017) (ANTARES) (Remove no-longer needed ATB, KSC, Patrick interface))
    )
**********************************************************************************/

#include "jeod/models/environment/time/include/time_standard.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/atmosphere_exec.hh"


/*******************************************************************************
Method:
Purpose: Constructor
Note: It's possible that gram_i_in and master_gram_in are identical pointers
      in the case where the vehicle's gram is the master gram.  In this case
      master_gram_in == NULL.
*******************************************************************************/
AtmosphereExec::AtmosphereExec(
         GramInterface         & gram_i_in,
         LookupAtmosWinds         & drwp_atmos_in,
         STD1976                  & std76_in,
         SimpleLookupWind         & simple_wind_in,
         const GramInterface   * master_gram_in,
         const double             & dyn_time_in,
         ExtendedPlanetaryDerivedState & state_in,
         jeod::DynBody        & body_in)
 :
  AtmosphereExecInterface(dyn_time_in,
                          state_in,
                          body_in),
  atmos_model(ATMOS_NONE),
  winds_model(WINDS_NONE),
  wind_hold(dyn_time,
            planet_state,
            out),
  use_gust(true),
  gust(dyn_time_in),
  gust_vector{0.0, 0.0, 0.0},
  Su(110),         // from std 76 atmosphere model
  beta(1.458E-6),  // from std 76 atmosphere model
  month_init(0),
  atmos_gram( gram_i_in,
              master_gram_in,
              planet_state,
              dyn_time,
              out),
  atmos_drwp( drwp_atmos_in,
              planet_state,
              out),
  atmos_std( std76_in,
             planet_state.state.ellip_coords.altitude,
             out),
  winds_simple( simple_wind_in,
                planet_state,
                out)
{
  jeod::Vector3::initialize(gust_vector);
}

/*******************************************************************************
Method:
Purpose: GRAM-less Constructor
Note: For use cases where GRAM isn't available, this constructor provides the
      necessary GramInterface stub to the underlying classes
*******************************************************************************/
AtmosphereExec::AtmosphereExec(
         LookupAtmosWinds         & drwp_atmos_in,
         STD1976                  & std76_in,
         SimpleLookupWind         & simple_wind_in,
         const double             & dyn_time_in,
         ExtendedPlanetaryDerivedState & state_in,
         jeod::DynBody        & body_in)
 :
  AtmosphereExecInterface(dyn_time_in,
                          state_in,
                          body_in),
  atmos_model(ATMOS_NONE),
  winds_model(WINDS_NONE),
  wind_hold(dyn_time,
            planet_state,
            out),
  use_gust(true),
  gust(dyn_time_in),
  gust_vector{0.0, 0.0, 0.0},
  Su(110),         // from std 76 atmosphere model
  beta(1.458E-6),  // from std 76 atmosphere model
  month_init(0),
  atmos_gram( gi_stub,
              NULL,
              planet_state,
              dyn_time,
              out),
  atmos_drwp( drwp_atmos_in,
              planet_state,
              out),
  atmos_std( std76_in,
             planet_state.state.ellip_coords.altitude,
             out),
  winds_simple( simple_wind_in,
                planet_state,
                out)
{
  jeod::Vector3::initialize(gust_vector);
}

/*****************************************************************************
activate
Purpose:(Activates the model, allows it to run)
*****************************************************************************/
void
AtmosphereExec::activate()
{
  AtmosphereExecInterface::activate(); // handles planet-state subscription
                                       // and active flag.
  if ( atmos_model == ATMOS_GRAM10 ||
       winds_model == WINDS_GRAM10) {
     atmos_gram.activate();
  }
  if ( atmos_model == ATMOS_DRWP ||
       winds_model == WINDS_DRWP ) {
     atmos_drwp.activate();
  }
  if (winds_model == WINDS_SIMPLE) {
     winds_simple.activate();
  }

  update();

  // Activate the wind-hold model
  wind_hold.activate();
}

/*****************************************************************************
deactivate
Purpose:(Deactivates the model, cancels subscriptions)
*****************************************************************************/
void
AtmosphereExec::deactivate()
{
  if (active) {
    if (atmos_model == ATMOS_GRAM10 ||
        winds_model == WINDS_GRAM10) {
      atmos_gram.deactivate( );
    }
    if ( atmos_model == ATMOS_DRWP ||
         winds_model == WINDS_DRWP ) {
      atmos_drwp.deactivate( );
    }
    if (winds_model == WINDS_SIMPLE) {
      winds_simple.deactivate();
    }

    // Turn off wind-hold.
    wind_hold.deactivate();
    AtmosphereExecInterface::deactivate(); // handles planet-state subscription
                                           // and active flag.
  }
}

/*******************************************************************************
Method:initialize
Purpose:initializes the atmos_exec class
*******************************************************************************/
void
AtmosphereExec::initialize( // Return: --   Void
  jeod::TimeStandard & time_in)         // --  UT1 clock

{
  if (!enabled) {
    return;
  }

  // month_init is typically set externally to allow for dispersions.
  // If it has not been set, set it off the calendar.
  if (month_init == 0) {
    month_init = time_in.calendar_month;
  }

  if (atmos_model == ATMOS_GRAM10 ||
      winds_model == WINDS_GRAM10) {
    atmos_gram.set_time( time_in.calendar_year,
                         month_init,
                         time_in.calendar_day,
                         time_in.calendar_hour,
                         time_in.calendar_minute,
                         time_in.calendar_second);
  }

  // Initialize the specific atmosphere and wind model
  initialize_atmosphere();
  initialize_winds();

  AtmosphereExecInterface::initialize();

  CMLMessage::inform(
    __FILE__,__LINE__," Atmosphere Exec initialization configuration:\n",
    "For vehicle ", body.name,
    ":\nAtmosphere: ", current_atmos->name,
    "\nWinds: ", current_winds->name,
    "\nTime: ", time_in.calendar_year, "/",
    CMLMessage::printf_fmt("%02d", month_init), "/",
    CMLMessage::printf_fmt("%02d", time_in.calendar_day), " :: ",
    CMLMessage::printf_fmt("%02d", time_in.calendar_hour), ":",
    CMLMessage::printf_fmt("%02d", time_in.calendar_minute), ":",
    CMLMessage::printf_fmt("%05.2f", time_in.calendar_second));
}

/*****************************************************************************
initialize_atmosphere
Purpose:(initializes the atmosphere)
*****************************************************************************/
void
AtmosphereExec::initialize_atmosphere()
{
  switch (atmos_model) {

  case ATMOS_NONE:
    current_atmos = &atmos_none;
    break;

  case ATMOS_STD76:
    current_atmos = &atmos_std;
    break;

  case ATMOS_GRAM10:
    current_atmos = &atmos_gram;
    break;

  case ATMOS_DRWP:
    current_atmos = &atmos_drwp;
    break;

  default:
    CMLMessage::fail(
    __FILE__,__LINE__,"Invalid initialization.\n",
    "Invalid atmosphere option has been selected\n");
    break;
  }
}

/*****************************************************************************
initialize_winds
Purpose:(initializes the winds model)
*****************************************************************************/
void
AtmosphereExec::initialize_winds()
{
  switch (winds_model) {

  case WINDS_NONE:
    current_winds = &atmos_none;
    break;

  case WINDS_GRAM10:
    current_winds = &atmos_gram;
    break;

  case WINDS_DRWP:
    current_winds = &atmos_drwp;
    break;

  case WINDS_SIMPLE:
    current_winds = &winds_simple;
    break;

  default:
    CMLMessage::fail(
    __FILE__,__LINE__,"Invalid initialization.\n",
    "Invalid winds option has been selected\n");
    break;
  }
}

/*******************************************************************************
Method: update
Purpose: updates atmosphere and winds
*******************************************************************************/
void
AtmosphereExec::update() // Return: --   Void
{
  if (!active) {
    return;
  }

  // NULL pointer checks: pointers are protected and are verified before the
  // active flag can be set (which is also protected and can only be set if
  // initialized; check is made as part of initialization routine.)
  current_atmos->update_atmos();
  current_winds->update_winds();

  if (use_gust) {
    add_gust();
  }

  out.atmos_new_time = dyn_time;

  // estimate the atmospheric mean-free-path;
  // Equation taken from GRAM99 (models.f)
  if ( current_atmos != &atmos_none) {
    out.mean_free_path = MathUtils::divide_protected(2.3025e-05*out.temperature,
                                                     out.pressure,
                                                     out.mean_free_path);
  }

  // Estimate dynamic viscosity if it is not computed in the selected
  // atmosphere model
  // Equation and constants taken from standard 76 atmosphere model
  if ( current_atmos != &atmos_none &&
       current_atmos != &atmos_std ) {
    out.dynamic_viscosity = MathUtils::divide_protected(beta*std::pow(out.temperature,1.5),
                                                        out.temperature + Su,
                                                        out.dynamic_viscosity);
  }
}

/*******************************************************************************
Purpose:  switch from GRAM to DRWP table winds at low altitude
*******************************************************************************/
// FIXME Stachowiak/Turner May 2017
//       Investigate whether the existing atmosphere should be deactivated with
//       the removal of its winds dependency. Decision to do so would be based
//       on whether the atmosphere is still in use (testable) and whether the
//       atmosphere is desired to still run (user input).

void
AtmosphereExec::switch_to_drwp_winds() // Return: --   Void
{
  winds_model = WINDS_DRWP;
  atmos_drwp.activate();
  initialize_winds();
  atmos_drwp.initialize_winds();
}

/*****************************************************************************
add_gust
Purpose:(adds the gust to the outputs)
*****************************************************************************/
void
AtmosphereExec::add_gust()
{
  gust.update(); /* Call the 1 minus cosine gust model to update gust values */
  if (!gust.is_active()) return;

  switch(gust.gust_frame) {
  case GustModel::TD_FRAME:
    /* No transformation needed; topodetic frame is the target frame. */
    jeod::Vector3::copy(gust.gust_vector, gust_vector);
    break;
  case GustModel::BODY_FRAME:
    /* Take the gust vector in body frame into topodetic frame. */
    jeod::Vector3::transform_transpose( planet_state.topodetic.T_this_to_body,
                                  gust.gust_vector,
                                  gust_vector);
    break;
  default:
    /* Initialize the gust vector with the null vector and turn gust off.*/
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid gust frame specification\n",
      "Invalid specification for gust.gust_frame.\n"
      "Setting gust to zero and turning off the model.\n");
    jeod::Vector3::initialize(gust_vector);
    use_gust = false;
    return;
    break;
  }

  jeod::Vector3::incr( gust_vector, out.wind_velocity_td);
  jeod::Vector3::transform_transpose( planet_state.topodetic.T_inrtl_to_this,
                                out.wind_velocity_td,
                                out.wind_velocity_eci);
  jeod::Vector3::transform( planet_state.topocentric.T_inrtl_to_this,
                      out.wind_velocity_eci,
                      out.wind_velocity_tc);
  out.wind_vmag = jeod::Vector3::vmag( out.wind_velocity_td );
}
