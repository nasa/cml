/*******************************************************************************
Purpose:
   (Wrapper around a single hifi parachute instance.)

Library Dependency:
   ((hifi_parachute.o))
*******************************************************************************/

#ifndef HIFI_PARACHUTE_HH
#define HIFI_PARACHUTE_HH

#include "parachute_exec.hh"
#include "jeod/models/dynamics/mass/include/mass.hh"
#include "jeod/models/dynamics/body_action/include/body_attach_matrix.hh"
#include "jeod/models/dynamics/body_action/include/body_detach.hh"
#include "cml/models/dynamics/mass/mass_body_dispersed_init/include/mass_body_dispersed_init.hh"
#include "cml/models/environment/atmos/lagged_atmosphere/include/lagged_atmos_wind.hh"
#include "cml/models/environment/atmos/lagged_atmosphere/include/lagged_atmos_payload_data.hh"
#include "jeod/models/utils/sim_interface/include/trick_dynbody_integ_loop.hh"
#include "jeod/models/environment/planet/include/planet.hh"
#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"
#include "cml/models/environment/atmos/atmos_exec/include/atmosphere_exec_out.hh"
#include "cml/models/dynamics/state_descriptors/extended_planetary_derived_state/include/extended_planetary_derived_state.hh"
#include "cml/models/dynamics/state_descriptors/atmos_rel_state/include/atmos_relative_state.hh"

#include "parachute_integrable_object.hh"

class HifiParachute {

    public:

        bool deploy;
        bool release;

        PARACHUTE_DATA data;

        LaggedAtmosWind         lagged_atmos_wind;
        LaggedAtmosPayloadData  payload_data;

        double force[3];  /* (N) Force from chute model, includes mortar force */
        double moment[3]; /* (N*m) Moment from chute model, includes mortar moment and riser twist torque */

        HifiParachute();
        virtual ~HifiParachute();

        void create_integrable_objects();
        void remove_integrable_objects(jeod::JeodDynbodyIntegrationLoop & integ_loop);
        void add_integrable_objects(jeod::JeodDynbodyIntegrationLoop & integ_loop);

        void compute_derivatives(const bool fbcp_deploy,
                                 const double time,
                                 const jeod::DynBody & body,
                                 const AtmosExecOutput & atmos_exec,
                                 const ExtendedPlanetaryDerivedState & planet_rel_state,
                                 const AtmosRelativeState & atmos_rel);

        void update(const jeod::DynBody & body,
                    const AtmosExecOutput & atmos_exec_payload,
                    const ExtendedPlanetaryDerivedState & planet_rel_state,
                    const jeod::Planet & planet);

    private:

        std::list<ParachuteIntegrableObject *> integrable_objects;

        int deploy_int() { return deploy ? 1 : 0; };
        int release_int() { return release ? 1 : 0; };

        bool fbcp_deployed;

        bool integ_object_created;

        void collect();

        void chute_data_mapping_deriv(
                    const bool                           fbcp_deploy,
                    const double                         sim_time_local,
                    const jeod::DynBody&                 dyn_body,
                    const AtmosExecOutput&               atmos_exec,
                    const ExtendedPlanetaryDerivedState& planet_rel_state,
                    const AtmosRelativeState&            atmos_rel,
                    PARACHUTE_DATA&                      chute);

        void chute_data_mapping_effector(
                    const jeod::DynBody&                 dyn_body,
                    const AtmosExecOutput&               atmos_exec_payload,
                    const ExtendedPlanetaryDerivedState& planet_rel_state,
                    const jeod::Planet&                  pc,
                    PARACHUTE_DATA&                      chute);

        HifiParachute(const HifiParachute &);
        HifiParachute & operator=(const HifiParachute &);
};
#endif
