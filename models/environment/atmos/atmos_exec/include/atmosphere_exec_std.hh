/******************************** TRICK HEADER **********************************
PURPOSE:
   (Interface for the STD_76 atmosphere.
    Comprises atmosphere only; there is no associated winds model.)

LIBRARY DEPENDENCY:
    (
     (../src/atmosphere_exec_std.cc)
    )

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (May 2016) (ANTARES)
       (New architecture to isolate the atmospheres from the master executive))
    ((Bingquan Wang) (OSR) (May 2017) (ANATRES)
       (Tweaked the code due to the refactoring of std_atmos_1976 model))
   )
********************************************************************************/

#ifndef ATMOS_EXEC_STD_HH
#define ATMOS_EXEC_STD_HH

#include "cml/models/environment/atmos/atmosphere_models/std_atmos_1976/include/std_atmos_1976.hh"

#include "atmosphere_exec_atmos_base.hh"

class AtmosphereExec_STD : public AtmosphereExec_AtmosBase
{
  protected:
    STD1976 & std76;             /* (--) Standard 1976 atmosphere structure */
    const double & topodetic_altitude; /* (--) reference to topodetic altitude.*/

  public:
    double hmsl; /* (m) height above mean sea level. */

    AtmosphereExec_STD( STD1976 & std_76_in,
                        const double    & topodetic_alt,
                        AtmosExecOutput & master_output);
    virtual ~AtmosphereExec_STD(){};

    virtual bool initialize_atmos() override {return true;};
    virtual void update_atmos() override;

  private:
    AtmosphereExec_STD (const AtmosphereExec_STD&);
    AtmosphereExec_STD & operator = (const AtmosphereExec_STD&);
};
#endif
