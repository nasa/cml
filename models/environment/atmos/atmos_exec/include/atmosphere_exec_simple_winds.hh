/******************************** TRICK HEADER **********************************
PURPOSE:
   (Interface for the Simple Winds basic table-driven model.)

LIBRARY DEPENDENCY:
    (
     (../src/atmosphere_exec_simple_winds.cc)
    )

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (July 2018) (ANTARES)
       (New implementation))
   )
********************************************************************************/

#ifndef ATMOS_EXEC_SIMPLE_LOOKUP_WIND_HH
#define ATMOS_EXEC_SIMPLE_LOOKUP_WIND_HH

#include "cml/models/dynamics/state_descriptors/extended_planetary_derived_state/include/extended_planetary_derived_state.hh"
#include "cml/models/environment/atmos/atmosphere_models/simple_lookup_wind/include/simple_lookup_wind.hh"

#include "atmosphere_exec_atmos_base.hh"


class AtmosphereExec_SimpleLookupWind : public AtmosphereExec_AtmosWindsBase
{
 public:
  enum TopoType {
    Topodetic = 0,  // referenced to the ellipsoid; perpendicular
    Topocentric = 1,// referenced to the ellipsoid, pointing to planet-center
    Spherical = 2,  // referenced to the spheroid, pointing to planet-center
  };

 protected:
  // External references:
  SimpleLookupWind & wind_table; /* (--) Simple table-driven wind model. */

  // Internal working variables
 public:
  TopoType  wind_components_type; /* (--)
         specification of how to interpret the output data coming from the
         wind-table.  Default: Topodetic. */
 protected:
  TopoType altitude_type; /* (--)
         specification of how to interpret the altitude data in the
         wind-table.  Default: Topodetic. */
  double altitude_at_prev_update; /* (m)
         altitude at which table was last processed. */
  const double * altitude_ptr; /* (--)
         pointer to the appropriate altitude from planet_state, according to
         the setting of the altitude_type. */

  public:

    AtmosphereExec_SimpleLookupWind(
                         SimpleLookupWind              & wind_table,
                         ExtendedPlanetaryDerivedState & planet_state,
                         AtmosExecOutput               & master_output);
    virtual ~AtmosphereExec_SimpleLookupWind(){};

    virtual void activate() override;
    virtual void deactivate() override;
    virtual bool initialize_winds() override {wind_table.initialize();
                                              return true;}
    virtual void update_winds() override;

    void set_altitude_type(TopoType altitude_);

  private:
    AtmosphereExec_SimpleLookupWind (const AtmosphereExec_SimpleLookupWind&);
    AtmosphereExec_SimpleLookupWind & operator = (const AtmosphereExec_SimpleLookupWind&);
};
#endif
