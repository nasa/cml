/******************************** TRICK HEADER **********************************
PURPOSE:
   (Interface for the GRAM atmosphere.)

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (May 2016) (ANTARES) 
       (New architecture to isolate the atmospheres from the master executive))
   )
********************************************************************************/

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/atmosphere_exec_gram.hh"

// TODO: move these GramInterface lines to their own file?
GramInterface::GramInterface()
:
  out(),
  input(),
  cache()
{
}
void GramInterface::initialize( // Return: -- Void
 double ,
 double ,
 double ,
 double ,
 int    ,
 int    ,
 int    ,
 int    ,
 int    ,
 double )
{
  CMLMessage::fail( __FILE__,__LINE__,
    "Missing derived GramInterface\n",
    "GramInterface initialize() method has not been overridden which means GRAM\n"
    "does not exist for this AtmosphereExec_Gram instance. This base class method\n"
    "is used only as a non-functional stub to support the Atmosphere Exec architecture\n"
    "when GRAM is not available.\n");
    return;
}

void GramInterface::update(
   double ,
   double ,
   double ,
   double )
{
  CMLMessage::fail( __FILE__,__LINE__,
    "Missing derived GramInterface\n",
    "GramInterface update() method has not been overridden, which means GRAM\n"
    "does not exist for this AtmosphereExec_Gram instance. This base class method\n"
    "is used only as a non-functional stub to support the Atmosphere Exec architecture\n"
    "when GRAM is not available.\n");
    return;
}

/*****************************************************************************
Purpose:(Construct the class)
*****************************************************************************/
AtmosphereExec_Gram::AtmosphereExec_Gram(
     GramInterface & gram_in,
     const GramInterface * master_gram_10_in,
     ExtendedPlanetaryDerivedState & planet_state_in,
     const double & dyn_time_in,
     AtmosExecOutput & master_output)
  :
  AtmosphereExec_AtmosWindsBase(master_output, planet_state_in),
  gram_i( gram_in),
  master_gram( master_gram_10_in),
  dyn_time( dyn_time_in),
  activate_gram_from_master(true),
  initialized(false),
  year_init(0),
  month_init(0),
  day_init(0),
  hour_init(0),
  min_init(0),
  sec_init(0.0),
  last_update_time(-1.0e30) // A very large negative number
{
  name = "GRAM";
  if (master_gram == NULL || master_gram == &gram_i) {
    activate_gram_from_master = false;
  }
}

/*****************************************************************************
activate
Purpose:(Activates the model, creates necessary subscriptions)
*****************************************************************************/
void
AtmosphereExec_Gram::activate()
{
  if (active) return;

  // GRAM operation utilizes Fortran common blocks, which are assumed
  // populated with values appropriate to the current vehicle config.  It may
  // be necessary to populate these from an active set of common-block data
  // before running the GRAM model.
  if( activate_gram_from_master) {
    if (master_gram == NULL) {
      CMLMessage::error(
      __FILE__,__LINE__,"Atmos-exec activation error\n",
      "Flags set such that GRAM will be initialized from an external\n"
      "instance of GRAM at activation but the pointer to the external\n"
      "model is NULL.  Model will be initialized with default settings.\n");
    }
    else {
      // This represents the minimum set of data that is necessary for  the
      // appropriate application of dispersions.
      // At this point, we need to copy the data from the master-gram (C-side)
      // to the vehicle-specific local-gram (C-side).
      // When gram gets called at update(), these data will be copied to the
      // local-gram (Fortran-side), and from there, the dispersions will be
      // computed.
      // Comment from gturner on why this capability exists:
      //    it is to provide the model-configuration and possibly the state
      //    history, but we aren't sure about the state history part. Without
      //    this, two vehicles would get different configurations and
      //    consequently different atmosphere states even when they have the
      //    same spatial state
      // Note that this data includes things like like solar, seasonal activity,
      // these are things specific to the atmosphere, not the position of the vehicle
      // in the atmosphere, so this copy of data is really a way to initialize gram
      // with these "common" pieces of data from the master gram
      memcpy( gram_i.cache.iotemp_int,
              master_gram->cache.iotemp_int,
              sizeof(gram_i.cache.iotemp_int));
      memcpy( gram_i.cache.iotemp_dbl,
              master_gram->cache.iotemp_dbl,
              sizeof(gram_i.cache.iotemp_dbl));
      memcpy( gram_i.cache.timeo_int,
              master_gram->cache.timeo_int,
              sizeof(gram_i.cache.timeo_int));
      memcpy( gram_i.cache.timeo_dbl,
              master_gram->cache.timeo_dbl,
              sizeof(gram_i.cache.timeo_dbl));
      memcpy( gram_i.cache.rcset1_int,
              master_gram->cache.rcset1_int,
              sizeof(gram_i.cache.rcset1_int));
      memcpy( gram_i.cache.rcset1_dbl,
              master_gram->cache.rcset1_dbl,
              sizeof(gram_i.cache.rcset1_dbl));
      memcpy( gram_i.cache.vert_dbl,
              master_gram->cache.vert_dbl,
              sizeof(gram_i.cache.vert_dbl));
      memcpy( gram_i.cache.vert_int,
              master_gram->cache.vert_int,
              sizeof(gram_i.cache.vert_int));
      // This one is necessary for cases where the secondary gram is starting at
      // exactly the same location as the master gram.  In this case, the test
      // of whether the vehicle is in its old position results in the code
      // bypassing the gram-update method, resulting in pressure and density
      // remaining at zero, and a NaN coming into the sim for speed-of-sound.
      // So we need to copy the master's pressure, density, temperature, etc so
      // that these have values in case the execution gets bypassed.
      memcpy( gram_i.cache.wincom,
              master_gram->cache.wincom,
              sizeof(gram_i.cache.wincom));


      // The following is a list of additional GramCache versions of some of
      // the Fortran common blocks.  These appear to be not directly relevant
      // to dispersing the state, but the list is provided here for reference
      // and in case testing turns something up at a later date:
      // datacom
      // comper
      // scalecom
      // iucom
      // speccom
      // jaccon
      // srfdat
      // concom
      // pertinit_double
      // pertinit_int
    }
  }

  subscribe_to_topocentric_altitude();
  AtmosphereExec_AtmosBase::activate();
}

/*****************************************************************************
deactivate
Purpose:(Removes subscriptions)
*****************************************************************************/
void
AtmosphereExec_Gram::deactivate()
{
  if (active) {
    planet_state.unsubscribe_topocentric_altitude();
    AtmosphereExec_AtmosBase::deactivate();
  }
}

/*****************************************************************************
set_time
Purpose: Pass-through from the master AtmosExec to set the time needed by GRAM
*****************************************************************************/
void
AtmosphereExec_Gram::set_time( int year,
                               int month,
                               int day,
                               int hour,
                               int min,
                               double sec)
{
  // Set internal values from simulation instances
  // Typically, this time would come from UT1, solar time.
  // month_init is typically set externally to allow for dispersions.
  year_init  = year;
  month_init = month;
  day_init   = day;
  hour_init  = hour;
  min_init   = min;
  sec_init   = sec;
}

/*****************************************************************************
initialize
Purpose:(multi-purpose, used to initialize the atmosphere and the winds)
*****************************************************************************/
bool
AtmosphereExec_Gram::initialize()
{
  /* NOTE: other atmosphere models may require mean-sea-level altitude */
  /*       however GRAM requires WGS84 geocentric altitude data */
  if (!initialized) {
    gram_i.initialize( dyn_time,
                        planet_state.topocentric_altitude,
                        planet_state.state.sphere_coords.longitude,
                        planet_state.state.sphere_coords.latitude,
                        year_init,
                        month_init,
                        day_init,
                        hour_init,
                        min_init,
                        sec_init);
    initialized = true;
  }
  return true;
}

/*****************************************************************************
update
Purpose:(main update loop for atmos, interface to GRAM update, collects outputs)
*****************************************************************************/
void
AtmosphereExec_Gram::update_atmos()
{
  if (!MathUtils::is_near_equal(last_update_time, dyn_time)) {
    last_update_time = dyn_time;
    gram_i.update( dyn_time,
                    planet_state.topocentric_altitude,
                    planet_state.state.sphere_coords.longitude,
                    planet_state.state.sphere_coords.latitude);
  }

  out.density           = gram_i.out.atm_density;
  out.temperature       = gram_i.out.atm_temperature;
  out.pressure          = gram_i.out.atm_pressure;
  out.speed_of_sound    = gram_i.out.atm_speed_of_sound;
  out.dynamic_viscosity = 0.0;

  // GRAM speed of sound needs to be limited when at high altitude outside the
  // atmosphere.  If not, the mach number calculation in atmos_relative_state
  // breaks down.
  // The choice of these values, as explained by bhoelsch:
  //   The hard-coded speed of sound was a deliberate implementation for
  //   altitudes above the sensible atmosphere. The GRAM outputs for speed of
  //   sound end up producing a mach number that nears zero at altitudes above
  //   the atmosphere. This caused problems with triggers in the simulation that
  //   key off of mach number during entry. Prior to entry interface, we force
  //   the mach number to be the close to the entry interface value.
  if (planet_state.topocentric_altitude > 100000.0)  {
    out.speed_of_sound = 280.0;
  }
}

/*****************************************************************************
update
Purpose:(main update loop for winds, interface to GRAM update, collects outputs)
*****************************************************************************/
void
AtmosphereExec_Gram::update_winds()
{
  if (!MathUtils::is_near_equal(last_update_time, dyn_time)) {
    last_update_time = dyn_time;
    gram_i.update( dyn_time,
                    planet_state.topocentric_altitude,
                    planet_state.state.sphere_coords.longitude,
                    planet_state.state.sphere_coords.latitude);
  }
  out.wind_angle_blowing_from = gram_i.out.atm_wind_angle_blowing_from;
  out.wind_angle_blowing_to   = gram_i.out.atm_wind_angle_blowing_to;
  out.wind_vmag               = gram_i.out.atm_wind_vmag;
  out.wind_velocity_tc[0]     = gram_i.out.atm_wind_velocity_tc[0];
  out.wind_velocity_tc[1]     = gram_i.out.atm_wind_velocity_tc[1];
  out.wind_velocity_tc[2]     = gram_i.out.atm_wind_velocity_tc[2];

  winds_from_topocentric();
}