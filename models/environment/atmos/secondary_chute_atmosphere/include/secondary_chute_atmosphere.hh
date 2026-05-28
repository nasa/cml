/*********************************************************************************
PURPOSE:
  (Provide the class to facilitate adding an extra atmosphere to a vehicle for
  the purposes of modeling the chutes.)

LIBRARY DEPENDENCY:
    ((secondary_chute_atmosphere.o))

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (2/2015) (ANTARES) (Initial version))
   )

*********************************************************************************/
#ifndef ANTARES_CHUTE_ATMOSPHERE_HH
#define ANTARES_CHUTE_ATMOSPHERE_HH

#include "jeod/models/environment/time/include/time_ut1.hh"
#include "cml/models/fhw/effectors/chute/include/parachute_pre_atmos.hh"
#include "cml/models/dynamics/state_descriptors/extended_planetary_derived_state/include/extended_planetary_derived_state.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "cml/models/environment/atmos/atmos_exec/include/atmosphere_exec.hh"

class ChuteAtmosphere : public SubscriptionBase {
 public:
  ExtendedPlanetaryDerivedState   planet_rel_state; /* (--)
          the planet-relative state of the chute.
          NOTE: this is not computed, this class is here purely as a
                convenient data-storage.*/

  AtmosphereExec     atmos_exec; // (--) atmos-exec specific to the chute.
 protected:
  ExtendedPlanetaryDerivedState & veh_state; /* (--)
              state of the vehicle, not of the chute.  Computed externally.*/
  State_Atmos_Output           & chute_state; /* (--)
              partial-state of the chutes.  Based off the vehicle-state.
              Computed externally.*/


 public:
  ChuteAtmosphere(
         LookupAtmosWinds        & DRWP_atmos_in,
         STD1976            & std_76_in,
         SimpleLookupWind   & simple_wind,
         GramInterface      & gram_in,
         const GramInterface & master_gram_i,
         double             & dyn_time_in,
         ExtendedPlanetaryDerivedState & veh_state_in,
         jeod::DynBody  & body_in,
         State_Atmos_Output& chute_state_in);
  virtual ~ChuteAtmosphere(){};

  void initialize( jeod::TimeStandard & time);

  void update ();
 protected:
  void populate_planet_rel_state();
  // NOTE - atmos_exec subscription will not activate planet-rel-state
  //        because p-r-s has never been initialized.  This is desired
  //        behavior.  P-r-s is a data holder only and should not be
  //        updated.
  virtual void activate();
  virtual void deactivate(){ active = false;
                             atmos_exec.unsubscribe();};

 private:
    ChuteAtmosphere (const ChuteAtmosphere&);
    ChuteAtmosphere & operator = (const ChuteAtmosphere&);
};

#endif
