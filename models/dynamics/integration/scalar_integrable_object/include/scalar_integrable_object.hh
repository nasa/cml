/*******************************************************************************
Purpose:
  (Allow an arbitrary scalar state to be integrated by JEOD.)

Library Dependency:
  ((../src/scalar_integrable_object.cc))
*******************************************************************************/

#ifndef CML_SCALAR_INTEGRABLE_OBJECT_HH
#define CML_SCALAR_INTEGRABLE_OBJECT_HH

#include "jeod/models/utils/integration/include/restartable_state_integrator.hh"
#include "er7_utils/integration/core/include/integrable_object.hh"

// These objects will automatically update the value of the variable you
// connect it to.

// You need to register these with the integration manager, either through the
// jeod::DynBody or through the JeodDynbodyIntegrationLoop object in the
// JeodIntegLoopSimObject

// Each object has an add_integrable_object method

class ScalarIntegrableObject : public er7_utils::IntegrableObject {
public:
  ScalarIntegrableObject( double &value_in,
                          double &derivative_in);
  ScalarIntegrableObject(const ScalarIntegrableObject &) = delete;
  ScalarIntegrableObject & operator = (const ScalarIntegrableObject &) = delete;

  void create_integrators(
      const er7_utils::IntegratorConstructor &generator,
      er7_utils::IntegrationControls &controls,
      const er7_utils::TimeInterface &time_if) override;

  void destroy_integrators() override;

  void reset_integrators() override;

  er7_utils::IntegratorResult integrate( double dyn_dt,
                                                 unsigned int target_stage) override;

private:
  #ifndef SWIG
  jeod::RestartableScalarFirstOrderODEIntegrator integrator;
  #endif

  double &value;      /* (--) Input value to be integrated. */
  double &derivative; /* (--) Time derivative of the value to be integrated. */
};
#endif
