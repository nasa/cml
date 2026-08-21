/******************************** TRICK HEADER **********************************
PURPOSE:
   (Input/output interface for the atmosphere executive.)

LIBRARY DEPENDENCY:
    (
     (../src/atmosphere_exec_interface.cc)
    )

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (May 2016) (ANTARES)
       (New architecture to minimize the complexity of the interface that has
       to be seen by other models.))
   )
********************************************************************************/

#ifndef CML_ATMOSPHERE_EXEC_INTERFACE_HH
#define CML_ATMOSPHERE_EXEC_INTERFACE_HH

#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"
#include "cml/models/dynamics/state_descriptors/extended_planetary_derived_state/include/extended_planetary_derived_state.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

#include "atmosphere_exec_out.hh"
#include "atmosphere_exec_atmos_base.hh"


/*****************************************************************************
AtmosphereExecInterface
Purpose:(Provided to give external models easy access to the subscription
         information for AtmosphereExec without having to worry about which
         AtmosphereExec is being used)
*****************************************************************************/
class AtmosphereExecInterface : public SubscriptionBase
{
  protected:
    // External references
    const double & dyn_time;           /* (--) Reference to JEOD's dynamic time */
    ExtendedPlanetaryDerivedState & planet_state; /* (--)
           vehicle's planet-relative-state. */
    const jeod::DynBody  & body;      /* (--) ref to the dyn-body */
    AtmosphereExec_AtmosBase * current_atmos; /* (--)
           Pointer to atmosphere model */
    AtmosphereExec_AtmosWindsBase * current_winds; /* (--)
           Pointer to winds model */

    // Internal members
    AtmosphereExec_AtmosWindsBase   atmos_none; /* (--)
           Default connection to "no atmosphere" for atmos and winds.*/
    bool subscribed_planet_state_topo_calcs; /* (--)
           internal flag to record subscriptions to planet-rel-state. */

  public:
    AtmosExecOutput   out;             /* (--) Atmos exec outputs */

    AtmosphereExecInterface(const double & dyn_time,
                            ExtendedPlanetaryDerivedState & planet_state,
                            const jeod::DynBody  & body);
    ~AtmosphereExecInterface() override = default;
    AtmosphereExecInterface (const AtmosphereExecInterface&) = delete;
    AtmosphereExecInterface & operator = (const AtmosphereExecInterface&) = delete;

    void initialize() override;

    std::string get_current_atmos_name() {
        std::string name = "N/A";
        if (current_atmos) { name = current_atmos->name; }
        return name;
    }
    std::string get_current_winds_name() {
        std::string name = "N/A";
        if (current_winds) { name = current_winds->name; }
        return name;
    }

  protected:
    void activate() override;
    void deactivate() override;
};

#endif
