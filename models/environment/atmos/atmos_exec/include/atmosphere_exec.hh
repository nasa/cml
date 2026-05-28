/******************************** TRICK HEADER **********************************
PURPOSE:
   (Data structure for atmosphere executive.)

LIBRARY DEPENDENCY:
    (
     (../src/atmosphere_exec.cc)
    )

PROGRAMMERS:
   (
    ((Brian Hoelscher) (NASA-EG4) (June 2006) (ARES) (Initial version))
    ((Pat Galvin) (ESCG) (July 2006) (ARES) (wind vector changes - rdlaa09011))
    ((Pat Galvin) (ESCG) (July 2006) (ARES) (Patrick AFB winds database - rdlaa08990))
    ((Pat Galvin) (ESCG) (December 2006) (ANTARES) (Add special wind profile - rdlaa09288))
    ((Pat Galvin) (ESCG) (December 2006) (ANTARES) (Add special atmos profile - rdlaa09456))
    ((Jeremy Rea) (NASA) (January  2007) (CEV) (RDLaa09504: Implement CEV aerodata subroutine V0.2 [Mean free path]))
    ((Pat Galvin) (ESCG) (December 2007) (ANTARES) (Add KSC winds - rdlaa09505))
    ((Brian Bihari) (ESCG) (December 2007) (ANTARES) (Implement GRAM 2007 - rdlaa09780))
    ((Jon Berndt) (ESCG/Jacobs) (02/08) (ANTARES) (Added new 1-cos gust model))
    ((Mark Kane)  (NASA) (March 2009)   (ANTARES)  (Added atmos_new_time for wind first order hold))
    ((Ryan Stillwater) (NASA-DFRC) (May 2009) (AGDL00002617) (Implement ATB Atmosphere and Winds))
    ((Brian Bihari) (ESCG) (08/10) (ANTARES) (Implement GRAM 2010 - AGDL200000284))
    ((Brian Bihari) (ESCG) (08/11) (ANTARES) (Implement GRAM Month  AGDL200000656))
    ((Gary Turner) (OSR) (2014) (ANTARES) (Upgrade to C++ for Trick13))
    ((Gary Turner) (OSR) (2016) (ANTARES)
              (Refactor to make the individual atmos-execs add-ons and
               provide a lightweight interface for external models to see))
    ((Dan Jordan) (JETS) (11/2017) (ANTARES) (Remove no-longer needed ATB, KSC, Patrick interface))
   )
********************************************************************************/

#ifndef ATMOS_EXEC_HH
#define ATMOS_EXEC_HH


#include "cml/models/environment/atmos/gust/include/gust.hh"
#include "cml/models/environment/atmos/first_order_hold/include/first_order_hold.hh"

#include "atmosphere_exec_interface.hh"
#include "atmosphere_exec_drwp.hh"
#include "atmosphere_exec_gram.hh"
#include "atmosphere_exec_simple_winds.hh"
#include "atmosphere_exec_std.hh"

namespace jeod {
    class TimeStandard;
}

/*****************************************************************************
AtmosphereExec
Purpose:(The executive that contains references to all of the available winds
         for a given project.
         The design intent is that there are no direct dependencies on this
         class; other classes needing input should use the AtmosExecInterface
         class, and other classes needing output should use the
         AtmosExecOutput class.)
*****************************************************************************/
class AtmosphereExec : public AtmosphereExecInterface {
  public:

    enum AtmosphereOption {
      ATMOS_NONE      = 0, /* (--) No atmosphere */
      ATMOS_STD76     = 2, /* (--) U.S. Standard Atmosphere - 1976 */
      ATMOS_GRAM10    = 7, /* (--) Global Reference Atmosphere Model - 2010 */
      ATMOS_DRWP      = 8  /* (--) Doppler Radar Wind Pairs from SLS program */
    };

    enum WindsOption {
      WINDS_NONE    = 0, /* (--) No winds */
      WINDS_GRAM10  = 7, /* (--) Global Reference Atmosphere Model - 2010 */
      WINDS_DRWP    = 8, /* (--) Doppler Radar Wind Pairs from SLS program */
      WINDS_SIMPLE  = 9  /* (--) simple table-driven winds-only model. */
    };

    AtmosphereOption  atmos_model;     /* (--) Atmos model selection */
    WindsOption       winds_model;     /* (--) Winds model selection */
    FirstOrderHold    wind_hold;       /* (--) Smooths winds */
    bool              use_gust ;       /* (--) use the gust model*/
    GustModel         gust;            /* (--) Gust model parameters */
    double            gust_vector[3];  /* (--) Additional wind vector from gusts */

    int     Su;        /* (K)  Sutherland's Constant */
    double  beta;      /* (--) Constant [kg/(s*M*K^0.5)] */
    int     month_init;/* (--)  Atmos initial month input, user override */

    // Atmosphere and winds models
    AtmosphereExec_Gram               atmos_gram;
    AtmosphereExec_Drwp               atmos_drwp;
    AtmosphereExec_STD                atmos_std;
    AtmosphereExec_SimpleLookupWind   winds_simple;

    AtmosphereExec(
           GramInterface   & gram_in,
           LookupAtmosWinds   & DRWP_atmos_in,
           STD1976            & std_76_in,
           SimpleLookupWind   & simple_wind_in,
           const GramInterface * master_gram_in,
           const double       & dyn_time_in,
           ExtendedPlanetaryDerivedState & state_in,
           jeod::DynBody  & body_in);

    // GRAM-less constructor
    AtmosphereExec(
           LookupAtmosWinds   & DRWP_atmos_in,
           STD1976            & std_76_in,
           SimpleLookupWind   & simple_wind_in,
           const double       & dyn_time_in,
           ExtendedPlanetaryDerivedState & state_in,
           jeod::DynBody  & body_in);

    virtual ~AtmosphereExec(){};

    void initialize( jeod::TimeStandard & time_in);

    void update ();

    void switch_to_drwp_winds();

  protected:
    GramInterface gi_stub;  /* (--) Stub for GRAM-less implementations*/
    virtual void activate() override;
    virtual void deactivate() override;

    void initialize_atmosphere();
    void initialize_winds();
    void add_gust();

  private:
    AtmosphereExec (const AtmosphereExec&);
    AtmosphereExec & operator = (const AtmosphereExec&);
};

#endif
