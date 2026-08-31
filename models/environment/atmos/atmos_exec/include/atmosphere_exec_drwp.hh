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

#ifndef CML_ATMOSPHERE_EXEC_DRWP_HH
#define CML_ATMOSPHERE_EXEC_DRWP_HH

#include "atmosphere_exec_atmos_base.hh"

class AtmosExecOutput;
class ExtendedPlanetaryDerivedState;
class LookupAtmosWinds;

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
    ~AtmosphereExec_Drwp() override = default;
    AtmosphereExec_Drwp (const AtmosphereExec_Drwp&) = delete;
    AtmosphereExec_Drwp & operator = (const AtmosphereExec_Drwp&) = delete;

    void activate() override;
    void deactivate() override;
    bool initialize_atmos() override {return initialize();}
    bool initialize_winds() override {return initialize();}
    void update_atmos() override;
    void update_winds() override;

  protected:
    bool initialize();
};
#endif
