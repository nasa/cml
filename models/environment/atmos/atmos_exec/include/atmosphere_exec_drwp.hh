/******************************** TRICK HEADER **********************************
PURPOSE:
   (Interface for the DRWP atmosphere.)

LIBRARY DEPENDENCY:
    (
     (../src/atmosphere_exec_drwp.cc)
    )

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (May 2016) (ANTARES)
       (New architecture to isolate the atmospheres from the master executive))
   )
********************************************************************************/

#ifndef ATMOS_EXEC_DRWP_HH
#define ATMOS_EXEC_DRWP_HH

#include "cml/models/dynamics/state_descriptors/extended_planetary_derived_state/include/extended_planetary_derived_state.hh"
#include "cml/models/environment/atmos/atmosphere_models/DRWP_atmos/include/lookup_winds.hh"

#include "atmosphere_exec_atmos_base.hh"

class AtmosphereExec_Drwp : public AtmosphereExec_AtmosWindsBase
{
  protected:
    LookupAtmosWinds & DRWP_atmos; /* (--)
      Binary atmosphere-winds data reader based on Doppler Radar Wind values
      from SLS program */
    bool initialized; /* (--) Internal flag to indicate that initialize has run.*/
    double altitude_at_prev_update; /* (m)
            altitude at which DRWP was last updated. */

  public:
    AtmosphereExec_Drwp( LookupAtmosWinds              & DRWP_atmos_in,
                         ExtendedPlanetaryDerivedState & planet_state_in,
                         AtmosExecOutput               & master_output);
    virtual ~AtmosphereExec_Drwp(){};

    virtual void activate() override;
    virtual void deactivate() override;
    virtual bool initialize_atmos() override {return initialize();};
    virtual bool initialize_winds() override {return initialize();};
    virtual void update_atmos() override;
    virtual void update_winds() override;

  protected:
    bool initialize();

  private:
    AtmosphereExec_Drwp (const AtmosphereExec_Drwp&);
    AtmosphereExec_Drwp & operator = (const AtmosphereExec_Drwp&);
};
#endif
