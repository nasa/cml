/*******************************************************************************
Purpose:
   (Allow a parachute state to be integrated by JEOD.)

Library Dependency:
   ((parachute_integrable_object.o))
*******************************************************************************/

#include "../include/parachute_integrable_object.hh"

#include "trick/trick_math.h"

ParachuteIntegrableObject::ParachuteIntegrableObject(
    const bool   &init_complete,
    EOM_Outputs &output,
    EOM_Work    &work)
    :
    init_complete(init_complete),
    output(output),
    work(work)
{
}

void ParachuteIntegrableObject::create_integrators(
    const er7_utils::IntegratorConstructor &generator,
    er7_utils::IntegrationControls &controls,
    const er7_utils::TimeInterface &time_if)
{
    (void) time_if; //time_if is an unused parameter

    // Create the translational state integrator.
    trans_integrator.create_integrator(generator, controls);

    // Create the rotational state integrator.
    jeod::GeneralizedSecondOrderODETechnique::TechniqueType technique;
    technique = jeod::GeneralizedSecondOrderODETechnique::LieGroup;

    technique =
       jeod::GeneralizedSecondOrderODETechnique::validate_technique (
          generator, technique,
          __FILE__, __LINE__, "ParachuteIntegrableObject", "null");

    rot_integrator.create_integrator (technique, generator, controls);

   // Configure the object that merges results to use the integration technique
   // corresponding to the generator.
   integ_results_merger.configure (generator);

}

void ParachuteIntegrableObject::destroy_integrators()
{
    // Do nothing
}

void ParachuteIntegrableObject::reset_integrators()
{
    trans_integrator.reset_integrator();
    rot_integrator.reset_integrator();
}

/**
 * Integrate the translational state of a jeod::DynBody.
 * @param[in]     dyn_dt        Dynamic time step, in dynamic time seconds.
 * @param[in]     target_stage  The stage of the integration process
 *                              that the integrator should try to attain.
 * @return The status (time advance, pass/fail status) of the integration.
 */
er7_utils::IntegratorResult
ParachuteIntegrableObject::trans_integ (
   double dyn_dt,
   unsigned int target_stage)
{

   // Integrate the translational state.
   return trans_integrator.integrate (
             dyn_dt, target_stage,
             work.inertial_accel, output.inertial_vel, output.inertial_pos);
}

er7_utils::IntegratorResult
ParachuteIntegrableObject::rot_integ (
   double dyn_dt,
   unsigned int target_stage)
{

   // Integrate the rotational state.
   er7_utils::IntegratorResult status =
      rot_integrator.integrate (
         dyn_dt, target_stage,
         work.rot_accel,
         output.body_rates, work.quat_inertial_to_body);

   // Normalize the integrated quaternion.
   quat_norm_integ(work.temp_quat_inertial_to_body, work.quat_inertial_to_body);
   Q_COPY(work.quat_inertial_to_body, work.temp_quat_inertial_to_body);

   // Compute the corresponding transformation matrix.
   quat_to_mat(output.T_inertial_to_body, work.quat_inertial_to_body);

   return status;
}

er7_utils::IntegratorResult ParachuteIntegrableObject::integrate(
    double dyn_dt,
    unsigned int target_stage)
{
    er7_utils::IntegratorResult status (false);

    if (init_complete) { 
       integ_results_merger.merge_integrator_result(trans_integ (dyn_dt, target_stage), status); 
       integ_results_merger.merge_integrator_result(rot_integ (dyn_dt, target_stage), status); 
    }

    return status;
}
