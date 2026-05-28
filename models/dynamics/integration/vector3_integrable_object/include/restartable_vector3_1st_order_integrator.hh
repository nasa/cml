/************************** TRICK HEADER***************************************
PURPOSE:
(Define an generic 3-D 1st order integrator class for integrating 3-vectors)

REFERENCE:
(((JEOD integration/include/restartable_state_integrator.hh)))

PROGRAMMERS:
(((Gary Turner) (OSR) (October, 2015) (initial version)))
*******************************************************************************/

#ifndef ANTARES_RESTARTABLE_VECTOR3_INTEGRATOR_HH
#define ANTARES_RESTARTABLE_VECTOR3_INTEGRATOR_HH

#include "er7_utils/integration/core/include/first_order_ode_integrator.hh"
#include "jeod/models/utils/container/include/simple_checkpointable.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "jeod/models/utils/memory/include/jeod_alloc.hh"
#include "jeod/models/utils/integration/include/restartable_state_integrator_templates.hh"


class RestartableVector3FirstOrderODEIntegrator : public jeod::SimpleCheckpointable {
  public:

/*****************************************************************************
constructor
*****************************************************************************/
    RestartableVector3FirstOrderODEIntegrator ()
    :
    jeod::SimpleCheckpointable(),
    integrator(NULL),
    integrator_manager(integrator)
    {
      JEOD_REGISTER_CLASS (RestartableVector3FirstOrderODEIntegrator);
    }

/*****************************************************************************
destructor
*****************************************************************************/
    virtual ~RestartableVector3FirstOrderODEIntegrator () { };

/*****************************************************************************
create_integrator
Purpose:(
    * Create the integrator to be managed.
    * @param generator  Integrator constructor used to create the integrator.
    * @param controls   Integration controls to be passed to the generator.
    )
*****************************************************************************/
    void create_integrator (const er7_utils::IntegratorConstructor & generator,
                            er7_utils::IntegrationControls & controls)
    {
      integrator_manager.create_integrator (generator, controls);
    }

/*****************************************************************************
destroy_integrator
Purpose:(Destroy the integrator.)
*****************************************************************************/
    void destroy_integrator () { integrator_manager.destroy_integrator(); };

#ifndef SWIG
/*****************************************************************************
integrate
Purpose:(
    Propagate state to the specified stage of the integration
    process for an overall integration time interval of dyn_dt.

    Note that this is a pass-through to the encapsulated integrator object.
    See er7_utils::SecondOrderODEIntegrator::integrate for details.

    @param[in]     dyn_dt        Dynamic time step, in dynamic time seconds.
    @param[in]     target_stage  The stage of the integration process
                                 that the integrator should try to attain.
    @param[in]     accel         Time derivative of the generalized velocity.
    @param[in,out] velocity      Generalized velocity vector.
    @param[in,out] position      Generalized position vector.

    @return The status (time advance, pass/fail status) of the integration.
    )
*****************************************************************************/
    er7_utils::IntegratorResult integrate (
      double dyn_dt,
      unsigned int target_stage,
      double const * ER7_UTILS_RESTRICT deriv,
      double * ER7_UTILS_RESTRICT variable)
    ER7_UTILS_ALWAYS_INLINE
    {
      return integrator->integrate (dyn_dt, target_stage, deriv, variable);
    }
#endif

/*****************************************************************************
reset_integrator
Purpose:(
    Tell the integrator to reset itself. This should be called when the time
    step or time direction changes or upon a discrete change in state such
    docking/undocking. Such events invalidate saved state (if any).
    The integrator needs to restart from scratch when such events occur.
    )
*****************************************************************************/
    void reset_integrator () { integrator->reset_integrator(); }

/*****************************************************************************
simple_restore
Purpose:(Restore the integrator on restart.)
*****************************************************************************/
    virtual void simple_restore ()
    {
      integrator_manager.set_integrator_reference (integrator);
      integrator_manager.simple_restore ();
    }

    void restore_manager_integrator_reference()
    {
      integrator_manager.set_integrator_reference (integrator);
    }


  private:

    /*
    Pointer to the object that performs integration. The object is created
    managed by the integrator manager.
    */
    er7_utils::FirstOrderODEIntegrator * integrator; //!< trick_units(--)

    /*
    Object that creates and manages the integrator object.
    */
    jeod::RestartableFirstOrderODEIntegrator<3> integrator_manager; //!< trick_io(**)


    // Unimplemented member functions.  Copy constructor, operator =
    RestartableVector3FirstOrderODEIntegrator (
      const RestartableVector3FirstOrderODEIntegrator &);
    RestartableVector3FirstOrderODEIntegrator & operator= (
      const RestartableVector3FirstOrderODEIntegrator &);
};
#endif