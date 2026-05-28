/******************************** TRICK HEADER **********************************
PURPOSE:
   (Base-class interfaces for the different atmospheres.)

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (May 2016) (ANTARES)
       (New architecture to isolate the atmospheres from the master executive))
   )
********************************************************************************/

#ifndef ATMOS_EXEC_ATMOS_BASE_HH
#define ATMOS_EXEC_ATMOS_BASE_HH

#include <string>
#include "atmosphere_exec_out.hh"

#include "cml/models/dynamics/state_descriptors/extended_planetary_derived_state/include/extended_planetary_derived_state.hh"
#include "jeod/models/utils/math/include/vector3.hh"

/*******************************************************************************
AtmosphereExec_AtmosBase
Purpose:(Base class containing members and methods common to all atmosphere
         model interfaces)
*******************************************************************************/
class AtmosphereExec_AtmosBase
{
  protected:
    AtmosExecOutput & out; /* (--)
          Reference to output class from the master atmos-exec. */
    bool active; /* (--) internal flag, indicates whether activate has been run*/

  public:
    std::string name; /* (--) name of the atmosphere. */

    explicit AtmosphereExec_AtmosBase(AtmosExecOutput & master_output)
      :
      out(master_output),
      active(false),
      name("NONE")
    {};

    virtual ~AtmosphereExec_AtmosBase(){};

    virtual void activate(){active = true;};
    virtual void deactivate(){active = false;};
    // These initialize_*() methods could be pure virtual but then derived
    // classes that are missing either winds or atmos (winds-only or atmos-only
    // models) would need to provide empty corresponding initialize_*() methods.
    virtual bool initialize_atmos() {return false;}
    virtual void update_atmos(){};

  private:
    AtmosphereExec_AtmosBase (const AtmosphereExec_AtmosBase&);
    AtmosphereExec_AtmosBase & operator = (const AtmosphereExec_AtmosBase&);
};

/*******************************************************************************
AtmosphereExec_AtmosWindsBase
Purpose:(Base class containing members and methods common to all interfaces
         for atmosphere models that include winds)
*******************************************************************************/
class AtmosphereExec_AtmosWindsBase : public AtmosphereExec_AtmosBase
{
  protected:
    ExtendedPlanetaryDerivedState & planet_state;  /* (--) Reference to planet
                                                           state*/
  public:
    AtmosphereExec_AtmosWindsBase(
        AtmosExecOutput & master_output,
        ExtendedPlanetaryDerivedState & planet_state_in)
      :
      AtmosphereExec_AtmosBase(master_output),
      planet_state(planet_state_in)
    {};

    virtual bool initialize_winds() {return false;};
    virtual void update_winds(){};

  protected:
    // The planet-rel-state model should already be subscribed to and updated by
    // the atmos-exec 'manager-level' class.  But that update may have skipped
    // over the optional topocentric-altitude calculation, so when the wind
    // models subscribe to the topocentric-altitude calculation they need to
    // re-run the planet-state update if the topocentric altitude was not
    // already subscribed.
    void subscribe_to_topocentric_altitude() {
      bool already_subscribed = planet_state.is_topocentric_altitude_subscribed();
      planet_state.subscribe_topocentric_altitude();
      if (!already_subscribed) {
        planet_state.update();
      }
    }

    // Wind coordinate transformations
    void winds_from_topocentric() {
      jeod::Vector3::transform_transpose( planet_state.topocentric.T_inrtl_to_this,
                                          out.wind_velocity_tc,
                                          out.wind_velocity_eci);
      jeod::Vector3::transform( planet_state.topodetic.T_inrtl_to_this,
                                out.wind_velocity_eci,
                                out.wind_velocity_td);
    }
    void winds_from_topodetic() {
      jeod::Vector3::transform_transpose( planet_state.topodetic.T_inrtl_to_this,
                                          out.wind_velocity_td,
                                          out.wind_velocity_eci);
      jeod::Vector3::transform( planet_state.topocentric.T_inrtl_to_this,
                                out.wind_velocity_eci,
                                out.wind_velocity_tc);
    }

  private:
    AtmosphereExec_AtmosWindsBase (const AtmosphereExec_AtmosWindsBase&);
    AtmosphereExec_AtmosWindsBase & operator = (const AtmosphereExec_AtmosWindsBase&);
};
#endif
