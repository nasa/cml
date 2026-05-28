/*******************************************************************************
Purpose:
  (Allow an arbitrary scalar state to be integrated by JEOD.)

Programmers:
  (((Brent Caughron) (OSR) (Sept. 2017) (IV&V code review)))
*******************************************************************************/

#include "../include/scalar_integrable_object.hh"

/*******************************************************************************
Constructor
*******************************************************************************/
ScalarIntegrableObject::ScalarIntegrableObject( double &value_in,
                                                double &derivative_in)
  :
  value(value_in),
  derivative(derivative_in)
{}

/*******************************************************************************
create_integrators
Purpose: (
 * Create the first order integrator for this IntegrableObject.
 * @param generator  Integrator constructor that creates the integrator.
 * @param controls   Integration controls that mediate the integrations.
 * @param time_if    Integrator time variable
 )
*******************************************************************************/
void ScalarIntegrableObject::create_integrators(
    const er7_utils::IntegratorConstructor &generator,
    er7_utils::IntegrationControls &controls,
    const er7_utils::TimeInterface &time_if)
{
  integrator.create_integrator(generator, controls);
  (void) time_if;
}

/*******************************************************************************
destroy_integrators
Purpose: (Unused, here just for framework purposes to be used by another model.)
*******************************************************************************/
void ScalarIntegrableObject::destroy_integrators()
{
  // Do nothing
}

/*******************************************************************************
reset_integrators
Purpose: (Resets the integrator.)
*******************************************************************************/
void ScalarIntegrableObject::reset_integrators()
{
  integrator.reset_integrator();
}

/*******************************************************************************
integrate
Purpose: (
 * Call the integrator to do its thing
 * @param[in]     dyn_dt        Dynamic time step, in dynamic time seconds.
 * @param[in]     target_stage  The stage of the integration process
 *                              that the integrator should try to attain.
 * @return The status (time advance, pass/fail status) of the integration.
   )
*******************************************************************************/
er7_utils::IntegratorResult ScalarIntegrableObject::integrate(
    double dyn_dt,
    unsigned int target_stage)
{
  return integrator.integrate(dyn_dt, target_stage, &derivative, &value);
}
