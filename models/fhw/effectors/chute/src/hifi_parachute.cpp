/*******************************************************************************
Purpose:
   (Wrapper around a single hifi parachute instance.)

Library Dependency:
   ((hifi_parachute.o))
*******************************************************************************/

#include "../include/hifi_parachute.hh"

#include "jeod/models/utils/math/include/vector3.hh"
#include "trick/trick_math.h"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

HifiParachute::HifiParachute()
    :
    deploy(false),
    release(false),
    data()
{
    integ_object_created = false;
    jeod::Vector3::initialize(force);
    jeod::Vector3::initialize(moment);
}

HifiParachute::~HifiParachute()
{
    if (!integrable_objects.empty()) {
       std::list<ParachuteIntegrableObject *>::iterator it = integrable_objects.begin();
       for (; integrable_objects.end()!=it; ++it) {
           delete (*it);
           *it = NULL;
       }
    }
}

void HifiParachute::create_integrable_objects()
{

    if (integ_object_created) {
       CMLMessage::fail(__FILE__,__LINE__,"Information only: \n","chute integrable object already created \n");
       return;
    }

    if (data.param.pilot_chute_active) {
        for (int i = 0; i < data.param.num_chutes; i++) {
            ParachuteIntegrableObject * integrable_object = new ParachuteIntegrableObject(data.init_complete[i],
                                                                                          data.eom_pilot[i].output,
                                                                                          data.eom_pilot[i].work);
            integrable_objects.push_back(integrable_object);
        }
    }
    for (int i = 0; i < data.param.num_chutes; i++) {
        ParachuteIntegrableObject * integrable_object = new ParachuteIntegrableObject(data.init_complete[i],
                                                                                      data.eom_main[i].output,
                                                                                      data.eom_main[i].work);
        integrable_objects.push_back(integrable_object);
    }

    integ_object_created = true;
}

void HifiParachute::remove_integrable_objects(
    jeod::JeodDynbodyIntegrationLoop & integ_loop)
{
    for (std::list<ParachuteIntegrableObject *>::iterator it = integrable_objects.begin(); it != integrable_objects.end(); ++it) {
        integ_loop.remove_integrable_object(**it);
    }
}

void HifiParachute::add_integrable_objects(
    jeod::JeodDynbodyIntegrationLoop & integ_loop)
{
    for (std::list<ParachuteIntegrableObject *>::iterator it = integrable_objects.begin(); it != integrable_objects.end(); ++it) {
        integ_loop.add_integrable_object(**it);
    }
}

void HifiParachute::compute_derivatives(
    const bool fbcp_deploy,
    const double time,
    const jeod::DynBody & body,
    const AtmosExecOutput& atmos_exec,
    const ExtendedPlanetaryDerivedState & planet_rel_state,
    const AtmosRelativeState & atmos_rel)
{

    chute_data_mapping_deriv( fbcp_deploy,
                              time,
                              body,
                              atmos_exec,
                              planet_rel_state,
                              atmos_rel,
                              data);
    parachute_exec(release_int(),
                   data,
                   data);

    collect();
}

void HifiParachute::update(
    const jeod::DynBody & body,
    const AtmosExecOutput & atmos_exec_payload,
    const ExtendedPlanetaryDerivedState & planet_rel_state,
    const jeod::Planet & planet)
{
    chute_data_mapping_effector(body, atmos_exec_payload, planet_rel_state, planet, data);
    parachute_exec_init(deploy_int(),
                        data);

    // FIXME: 3-Mar-17 In other sims, I have to call parachute_exec before calling
    // parachute_phasing the first time.  If I try that here, the answer
    // changes.  Jeff is looking at this and will fix it in the future.  For
    // now, we won't confuse the re-organization with this issue.

    // compute_derivatives(time,
    //                     body,
    //                     atmos_exec,
    //                     planet_rel_state,
    //                     atmos_rel);

    parachute_phasing(data);
    parachute_twist_exec(data);

    collect();
}

void HifiParachute::collect()
{

   jeod::Vector3::sum(data.output.total_mortar_force, data.output.force_on_payload, force);

   double interim_moment[3];
   jeod::Vector3::sum(data.output.total_mortar_moment, data.output.moment_on_payload, interim_moment);
   jeod::Vector3::sum(data.twist.output.twist_torque_SR, interim_moment, moment);

}

void HifiParachute::chute_data_mapping_deriv(
    const bool                           fbcp_deploy,
    const double                         sim_time_local,
    const jeod::DynBody&                 dyn_body,
    const AtmosExecOutput&               atmos_exec,
    const ExtendedPlanetaryDerivedState& planet_rel_state,
    const AtmosRelativeState&            atmos_rel,
    PARACHUTE_DATA&                      chute)
{
  //Unload needed data from input references
   chute.sim_time_local                       = sim_time_local;
   jeod::Vector3::copy(dyn_body.mass.composite_properties.position, chute.input.cg);
   jeod::Vector3::copy(dyn_body.get_root_body()->grav_interaction.grav_accel, chute.input.gravaccel);
   jeod::Vector3::copy(dyn_body.composite_body.state.trans.position, chute.input.pyld_inertial_pos);
   jeod::Vector3::copy(dyn_body.composite_body.state.trans.velocity, chute.input.pyld_inertial_vel);
   jeod::Vector3::copy(dyn_body.composite_body.state.rot.ang_vel_this, chute.input.pyld_inertial_rate);
   jeod::Matrix3x3::copy(dyn_body.composite_body.state.rot.T_parent_this, chute.input.pyld_T_inertial_to_body);
   jeod::Matrix3x3::copy(dyn_body.mass.composite_properties.T_parent_this, chute.input.pyld_T_body_to_SR);
   jeod::Vector3::copy(dyn_body.composite_body.state.rot.Q_parent_this.vector, chute.input.pyld_Q_inertial_to_body_vector);
   chute.input.pyld_Q_inertial_to_body_scalar = dyn_body.composite_body.state.rot.Q_parent_this.scalar;
   chute.input.pyld_mass                      = dyn_body.get_root_body()->mass.composite_properties.mass;
   jeod::Matrix3x3::copy(planet_rel_state.state.planet->pfix.state.rot.T_parent_this, chute.input.eci2ecef);
   chute.input.pyld_geocen_alt                = planet_rel_state.topocentric_altitude;
   chute.input.pyld_geocen_lat                = planet_rel_state.state.sphere_coords.latitude;
   chute.input.pyld_geocen_lon                = planet_rel_state.state.sphere_coords.longitude;
   chute.input.pyld_Vmag_earthrel             = planet_rel_state.relative_vel_mag;
   jeod::Vector3::copy( atmos_exec.wind_velocity_tc,
                  chute.input.atm_wind_vel_NED);
   chute.input.atm_density                    = atmos_exec.density;
   chute.input.atm_sos                        = atmos_exec.speed_of_sound;
   jeod::Vector3::copy(atmos_rel.free_stream_body_vel, chute.input.pyld_VairB);
   chute.input.pyld_angle_of_attack_in        = atmos_rel.angle_of_attack;
   chute.input.pyld_angle_of_attack_total_in  = atmos_rel.total_angle_of_attack;
   chute.input.pyld_sideslip_in               = atmos_rel.angle_of_sideslip;
   chute.input.pyld_qbar                      = atmos_rel.dynamic_pressure;

   //Copy over parent body inertial pos and Inertail_to_Body Transformantion matrix
   //Needed for line sail model, it aligns chute to payload velocity vector during deployment
   jeod::Vector3::copy(dyn_body.get_root_body()->composite_body.state.trans.position, chute.input.parent_inertial_pos);
   jeod::Matrix3x3::copy(dyn_body.get_root_body()->composite_body.state.rot.T_parent_this, chute.input.parent_T_inertial_to_body);

   //Create a copy of fbc_deploy flag that stays latched on
   if (fbcp_deploy) fbcp_deployed = true;

   //Only use lagged atmosphere when enabled, can't use lagged atmosphere for FBCPs because FBC atmosphere isn't on
   //until FBC jettison and lagged code needs to have atmos data from *before* jettison in order to work. In any case there
   //never has been a separate GRAM instance for FBCPs, so this isn't an issue.
   if ((fbcp_deployed==true) && (chute.param.use_lagged_atmos == true) && (chute.all_chutes_released == false)) {
      lagged_atmos_wind.compute(chute.gram.output.geocen_alt_gram/*, payload_data*/);
      chute.input.atm_density = lagged_atmos_wind.data_out.density;
      chute.input.atm_sos = lagged_atmos_wind.data_out.speed_of_sound;
      jeod::Vector3::copy(lagged_atmos_wind.data_out.planetodetic_wind_velocity, chute.input.atm_wind_vel_NED);
   }
}

void HifiParachute::chute_data_mapping_effector(
    const jeod::DynBody & dyn_body,
    const AtmosExecOutput & atmos_exec_payload,
    const ExtendedPlanetaryDerivedState & planet_rel_state,
    const jeod::Planet & pc,
    PARACHUTE_DATA&                       chute)
{
  //Unload needed data from input references
   jeod::Vector3::copy(dyn_body.mass.composite_properties.position, chute.input.EI_cg);
   jeod::Matrix3x3::copy(dyn_body.mass.composite_properties.T_parent_this, chute.input.EI_pyld_T_body_to_SR);
   jeod::Vector3::copy(dyn_body.composite_body.state.rot.Q_parent_this.vector, chute.input.EI_pyld_Q_inertial_to_body_vector);
   chute.input.EI_pyld_Q_inertial_to_body_scalar       = dyn_body.composite_body.state.rot.Q_parent_this.scalar;
   jeod::Matrix3x3::copy(dyn_body.composite_body.state.rot.T_parent_this, chute.input.EI_pyld_T_inertial_to_body);
   jeod::Vector3::copy(dyn_body.composite_body.state.trans.position, chute.input.EI_pyld_inertial_pos);
   jeod::Vector3::copy(dyn_body.composite_body.state.trans.velocity, chute.input.EI_pyld_inertial_vel);
   jeod::Vector3::copy(dyn_body.composite_body.state.rot.ang_vel_this, chute.input.EI_pyld_inertial_rate);
   chute.gram.input.r_eq                      = pc.r_eq;
   chute.gram.input.e_ellip_sq                = pc.e_ellip_sq;
   chute.gram.input.r_pol                     = pc.r_pol;
   chute.gram.input.pc_ang_vel0               = pc.pfix.state.rot.ang_vel_this[0];
   chute.gram.input.pc_ang_vel1               = pc.pfix.state.rot.ang_vel_this[1];
   chute.gram.input.pc_ang_vel2               = pc.pfix.state.rot.ang_vel_this[2];

   if (chute.param.use_lagged_atmos == true) {
      payload_data.altitude = planet_rel_state.topocentric_altitude;
      payload_data.density = atmos_exec_payload.density;
      payload_data.speed_of_sound = atmos_exec_payload.speed_of_sound;
      jeod::Vector3::copy(atmos_exec_payload.wind_velocity_tc, payload_data.planetodetic_wind_velocity);
      lagged_atmos_wind.update_history(payload_data);
   }

}
