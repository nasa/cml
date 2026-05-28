/************************** TRICK HEADER***************************************
PURPOSE:
(Define an generic IntegrableObject class for integrating 3-vectors)

REFERENCE:
(((JEOD Thermal-rider model thermal_integrable_object)))

Library dependencies:
((../src/vector3_integrable_object.cc))

PROGRAMMERS:
(((Gary Turner) (OSR) (October, 2015) (initial version)))
*******************************************************************************/

#ifndef ANTARES_VECTOR3_INTEGRABLE_OBJECT_HH
#define ANTARES_VECTOR3_INTEGRABLE_OBJECT_HH

#include "er7_utils/integration/core/include/integrable_object.hh"
#include "er7_utils/integration/core/include/integrator_result.hh"
#include "jeod/models/utils/integration/include/restartable_state_integrator.hh"
#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

#include "restartable_vector3_1st_order_integrator.hh"


class Vector3IntegrableObject : public SubscriptionBase,
                                public er7_utils::IntegrableObject
{
  public:
    double variable[3]; /* (--) Arbitrary vector, but typically pos or vel. */
    double deriv[3];    /* (--) values of the derivatives (optional). */

  protected:
    const double * deriv_ptr; /* (--) pointer to the derivative (optional) */
    jeod::DynBody * dyn_body; /* (--)
          ptr to the dyn-body that will integrate this variable. */

  public:
    Vector3IntegrableObject();
    explicit Vector3IntegrableObject(jeod::DynBody & body_in);
    Vector3IntegrableObject(jeod::DynBody & body_in,
                            const double * deriv_ptr_in);

    virtual ~Vector3IntegrableObject();

    virtual void initialize();
  protected:
    void zero_arrays();
    virtual void activate();
    virtual void deactivate();
  public:

    // Required by IntegrableObject
    virtual void create_integrators (
      const er7_utils::IntegratorConstructor & generator,
      er7_utils::IntegrationControls & controls,
      const er7_utils::TimeInterface & time_if);

    // Destroy the integrators.
    virtual void destroy_integrators (){integrator.destroy_integrator();};

    // Reset the integrators.
    virtual void reset_integrators () {integrator.reset_integrator();};

    // Propagate state.
    virtual er7_utils::IntegratorResult integrate ( double dyn_dt,
                                                    unsigned int target_stage);


  private:

    // Member data
#ifndef SWIG
    RestartableVector3FirstOrderODEIntegrator integrator; //!< trick_units(--)
#endif

    // Assignment and copy constructors are private and unimplemented
    Vector3IntegrableObject& operator = (const Vector3IntegrableObject& rhs);
    Vector3IntegrableObject(const Vector3IntegrableObject& rhs);
};
#endif
